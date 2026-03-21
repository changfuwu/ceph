.. _rgw_bucket_object_limits:

==================================================
RGW Bucket Object Limitations and Optimizations
==================================================

Overview
========

The Ceph RADOS Gateway (RGW) can store a very large number of objects in a single
bucket. However, there are practical limitations and performance considerations
that administrators should be aware of when working with buckets containing millions
of objects.

This document explains:

- Hard limits on bucket objects
- Soft limits and performance thresholds
- Optimization strategies for large buckets
- Configuration parameters for performance tuning

Hard Limits
===========

Theoretical Maximum Objects per Bucket
---------------------------------------

RGW does not impose a hard limit on the total number of objects that can be stored
in a single bucket. The theoretical maximum is limited by:

- Available storage capacity in the RADOS cluster
- 64-bit unsigned integer limit for object counts (2^64 - 1 objects)
- Bucket quota settings (if configured)

Bucket Quota Limits
-------------------

Administrators can set quotas to limit the number of objects in a bucket:

.. prompt:: bash #

   radosgw-admin quota set --quota-scope=bucket --bucket=<bucket_name> --max-objects=<N>

By default, bucket quotas are disabled (``max_objects = -1``, meaning unlimited).

Default quota values can be configured in ``ceph.conf``:

.. code-block:: ini

   [client.rgw]
   rgw_bucket_default_quota_max_objects = -1  # -1 means unlimited
   rgw_bucket_default_quota_max_size = -1      # size in bytes, -1 means unlimited

To view current quota settings for a bucket:

.. prompt:: bash #

   radosgw-admin bucket stats --bucket=<bucket_name>

Listing Operation Limits
-------------------------

When listing objects in a bucket via S3 or Swift APIs, there is a hard limit on
the maximum number of objects returned per request:

- **Maximum objects per list request**: 25,000 objects

This limit is defined by ``bucket_list_objects_absolute_max`` in the RADOS driver.
For buckets with more than 25,000 objects, clients must use pagination with
continuation tokens (markers) to retrieve all objects.

Soft Limits and Performance Thresholds
=======================================

Optimal Objects per Bucket Index Shard
---------------------------------------

The primary performance consideration for large buckets is the bucket index.
Each bucket index shard can handle entries efficiently up to a certain threshold:

- **Recommended maximum**: 100,000 objects per shard
- **Default trigger for dynamic resharding**: Configurable via ``rgw_max_objs_per_shard``

The default value for ``rgw_max_objs_per_shard`` is typically 100,000.

When a bucket index shard exceeds this threshold, performance degrades because:

- Index operations (inserts, deletes, listings) take longer
- RADOS operations on larger index objects consume more resources
- Listing operations require reading more data per shard

Maximum Bucket Index Shards
----------------------------

To distribute objects across multiple shards and maintain performance, buckets
can be sharded. The maximum number of shards is:

- **Maximum dynamic shards**: 1,999 (default, configurable via ``rgw_max_dynamic_shards``)
- **Absolute maximum shards**: 65,521 (``RGW_SHARDS_PRIME_1``)

With the recommended 100,000 objects per shard and maximum 65,521 shards, the
practical limit for a single bucket with optimal performance is approximately:

**6.5 billion objects** (65,521 shards × 100,000 objects/shard)

This is a soft limit - buckets can exceed this, but performance may degrade.

Optimization Strategies
=======================

1. Dynamic Bucket Index Resharding
-----------------------------------

.. versionadded:: Luminous

RGW supports automatic dynamic resharding to maintain performance as buckets grow.
Dynamic resharding is enabled by default and works transparently:

Enable dynamic resharding (enabled by default):

.. code-block:: ini

   [client.rgw]
   rgw_dynamic_resharding = true
   rgw_max_objs_per_shard = 100000
   rgw_max_dynamic_shards = 1999

When the number of objects per shard exceeds ``rgw_max_objs_per_shard``, RGW
automatically triggers a resharding operation to increase the number of shards.

**Note**: Dynamic resharding was limited in multisite deployments prior to Reef.
See :ref:`rgw_dynamic_bucket_index_resharding` for details.

.. versionadded:: Tentacle

Starting with Tentacle, dynamic resharding can also reduce the number of shards
when object counts decrease, with a configurable delay to avoid thrashing:

.. code-block:: ini

   [client.rgw]
   rgw_dynamic_resharding_may_reduce = true
   rgw_dynamic_resharding_reduction_wait = 86400  # seconds (24 hours)

2. Pre-Sharding Buckets
-----------------------

For workloads where you know a bucket will contain millions of objects,
pre-sharding the bucket avoids the overhead of multiple resharding operations:

.. prompt:: bash #

   radosgw-admin bucket reshard --bucket=<bucket_name> --num-shards=<N>

**Choosing the number of shards**:

1. Estimate the peak number of objects the bucket will contain
2. Divide by 100,000 (recommended objects per shard)
3. Choose a prime number near this value for better distribution

For example, for a bucket expected to contain 10 million objects:

- 10,000,000 / 100,000 = 100 shards
- Nearest prime number: 101

.. prompt:: bash #

   radosgw-admin bucket reshard --bucket=mybucket --num-shards=101

**Common prime shard counts**: 7, 11, 23, 53, 101, 211, 503, 1009, 2003, 5003, 10007

3. Setting Minimum Shard Count
------------------------------

.. versionadded:: Tentacle

To prevent automatic shard reduction below a certain threshold:

.. prompt:: bash #

   radosgw-admin bucket set-min-shards --bucket=<bucket_name> --num-shards=<min_shards>

4. Monitoring Bucket Index Performance
--------------------------------------

Monitor bucket statistics to identify when resharding is needed:

.. prompt:: bash #

   radosgw-admin bucket stats --bucket=<bucket_name>

Check resharding status:

.. prompt:: bash #

   radosgw-admin reshard status --bucket=<bucket_name>

List pending resharding operations:

.. prompt:: bash #

   radosgw-admin reshard list

**Automated Monitoring Script**:

A helper script is available at ``src/rgw/rgw_bucket_limits_check.sh`` to automate
bucket analysis and provide resharding recommendations:

.. prompt:: bash #

   # Check a specific bucket
   ./src/rgw/rgw_bucket_limits_check.sh -b mybucket

   # Check all buckets with warnings
   ./src/rgw/rgw_bucket_limits_check.sh -a -w

   # Check all buckets with custom threshold
   ./src/rgw/rgw_bucket_limits_check.sh -a -t 50000

The script requires ``radosgw-admin`` to be available in PATH and optionally uses
``jq`` for JSON parsing (falls back to grep/sed if jq is not available).

5. Optimizing Listing Operations
---------------------------------

For applications that frequently list bucket contents:

**Use prefix-based organization**:

Organize objects with common prefixes (similar to directory structure) to allow
efficient prefix-based listing:

.. code-block:: python

   # S3 example: list only objects with specific prefix
   response = s3.list_objects_v2(
       Bucket='mybucket',
       Prefix='2024/03/',
       MaxKeys=1000
   )

**Use pagination efficiently**:

Always use pagination for large buckets rather than trying to list all objects:

.. code-block:: python

   # S3 example: paginate through all objects
   paginator = s3.get_paginator('list_objects_v2')
   for page in paginator.paginate(Bucket='mybucket'):
       for obj in page.get('Contents', []):
           process_object(obj)

**Consider alternative designs**:

For extremely large datasets, consider:

- Multiple smaller buckets instead of one massive bucket
- External metadata indexing (e.g., in a database) for search/filtering
- Object naming schemes that encode metadata for filtering

Configuration Reference
========================

Key configuration parameters for bucket object limits and performance:

Quota Settings
--------------

.. code-block:: ini

   [client.rgw]
   # Default quota for new buckets (-1 = unlimited)
   rgw_bucket_default_quota_max_objects = -1
   rgw_bucket_default_quota_max_size = -1

   # User-level quotas
   rgw_user_default_quota_max_objects = -1
   rgw_user_default_quota_max_size = -1

   # Account-level quotas
   rgw_account_default_quota_max_objects = -1
   rgw_account_default_quota_max_size = -1

   # Quota cache TTL (seconds)
   rgw_bucket_quota_ttl = 600
   rgw_user_quota_bucket_sync_interval = 180
   rgw_user_quota_sync_interval = 86400

Dynamic Resharding Settings
----------------------------

.. code-block:: ini

   [client.rgw]
   # Enable automatic dynamic resharding
   rgw_dynamic_resharding = true

   # Trigger resharding when shard exceeds this many objects
   rgw_max_objs_per_shard = 100000

   # Maximum shards for dynamic resharding
   rgw_max_dynamic_shards = 1999

   # Allow shard count reduction (Tentacle+)
   rgw_dynamic_resharding_may_reduce = true
   rgw_dynamic_resharding_reduction_wait = 86400  # 24 hours

   # Resharding thread settings
   rgw_reshard_thread_interval = 600  # seconds between checks
   rgw_reshard_bucket_lock_duration = 360  # seconds

   # Resharding progress monitoring
   rgw_reshard_num_logs = 16
   rgw_reshard_progress_judge_interval = 60  # seconds
   rgw_reshard_progress_judge_ratio = 0.3  # 30% progress expected

Bucket Index Settings
---------------------

.. code-block:: ini

   [client.rgw]
   # Default number of shards for new buckets
   rgw_override_bucket_index_max_shards = 11

   # Minimum number of entries to read-ahead during listing
   rgw_list_bucket_min_readahead = 1000

Performance Tuning
------------------

.. code-block:: ini

   [client.rgw]
   # Maximum chunk size for listing operations
   rgw_list_buckets_max_chunk = 1000

   # Number of concurrent async operations
   rgw_multi_obj_del_max_aio = 16

   # Cache settings
   rgw_cache_enabled = true
   rgw_cache_lru_size = 10000

Best Practices Summary
======================

1. **Monitor object counts**: Regularly check bucket statistics to track growth

2. **Enable dynamic resharding**: Keep ``rgw_dynamic_resharding = true`` (default)

3. **Pre-shard large buckets**: If you expect millions of objects, pre-shard to
   avoid multiple resharding operations

4. **Choose prime shard counts**: Prime numbers provide better object distribution

5. **Use quotas for multi-tenant environments**: Set reasonable quotas to prevent
   runaway bucket growth

6. **Optimize application patterns**: Use prefixes, pagination, and consider
   multiple buckets for very large datasets

7. **Plan for scale**: Aim for ≤100,000 objects per shard for optimal performance

8. **Monitor resharding**: Check ``radosgw-admin reshard list`` periodically

9. **Consider workload patterns**: For write-heavy workloads with many objects,
   pre-sharding is especially beneficial

10. **Test at scale**: Test your expected workload before deploying to production

Troubleshooting
===============

Bucket Listing is Slow
----------------------

**Symptoms**: Listing operations take seconds or minutes

**Causes**:
- Too many objects per shard (>100,000)
- Bucket needs resharding
- Too many pending index transactions

**Solutions**:

1. Check bucket statistics:

   .. prompt:: bash #

      radosgw-admin bucket stats --bucket=<bucket_name>

2. Check if resharding is needed:

   .. prompt:: bash #

      radosgw-admin reshard status --bucket=<bucket_name>

3. Manually trigger resharding:

   .. prompt:: bash #

      radosgw-admin bucket reshard --bucket=<bucket_name> --num-shards=<N>

Object Uploads Failing with Quota Errors
-----------------------------------------

**Symptoms**: PUT operations return 413 (Quota Exceeded)

**Causes**:
- Bucket quota limit reached
- User quota limit reached
- Account quota limit reached

**Solutions**:

1. Check quota settings:

   .. prompt:: bash #

      radosgw-admin bucket stats --bucket=<bucket_name>
      radosgw-admin user info --uid=<user_id>

2. Increase quota if appropriate:

   .. prompt:: bash #

      radosgw-admin quota set --quota-scope=bucket --bucket=<bucket_name> --max-objects=<higher_limit>

3. Review and clean up old objects if quota is intentional

Resharding Stuck or Taking Too Long
------------------------------------

**Symptoms**: Resharding operation shows as "in-progress" for extended period

**Causes**:
- Very large bucket with millions of objects
- High I/O load on cluster
- Lock contention

**Solutions**:

1. Check resharding status:

   .. prompt:: bash #

      radosgw-admin reshard status --bucket=<bucket_name>

2. Monitor cluster I/O and wait for completion

3. If truly stuck, check RGW logs for errors

4. In extreme cases, cancel and retry:

   .. prompt:: bash #

      radosgw-admin reshard cancel --bucket=<bucket_name>
      # Wait a moment, then retry
      radosgw-admin bucket reshard --bucket=<bucket_name> --num-shards=<N>

   **Note**: Cancellation only works before resharding transitions to "in-progress"

See Also
========

- :ref:`rgw_dynamic_bucket_index_resharding` - Detailed resharding documentation
- :ref:`radosgw-admin-guide` - Admin operations and quota management
- :doc:`/dev/radosgw/bucket_index` - Developer documentation on bucket index internals
