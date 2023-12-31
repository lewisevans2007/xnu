#ifndef libTrustCache_API_h
#define libTrustCache_API_h

#include <sys/cdefs.h>
__BEGIN_DECLS

#include <stdint.h>
#include <stdbool.h>
#include <img4/firmware.h>
#include <TrustCache/RawTypes.h>
#include <TrustCache/Types.h>
#include <TrustCache/TypesConfig.h>
#include <TrustCache/Return.h>

/**
 * NOTE: This library does not enforce any concurrency by itself. To be safe in a multi-threaded
 * environment, the caller must manually enforce concurrency on the runtime data structure as
 * otherwise the library is susceptible to memory corruption from race conditions.
 */

/**
 * Initialize a runtime to the default values.
 *
 * If the system supports read-only segments, and the runtime is allocated within the read-only
 * segment, then this function needs to be called before the segment is enforced to be read-only.
 * For more information, please look at <TrustCache/Types.h>.
 */
static inline void
trustCacheInitializeRuntime(TrustCacheRuntime_t *runtime,
                            TrustCacheMutableRuntime_t *mutableRT,
                            bool allowSecondStaticTC,
                            bool allowEngineeringTC,
                            bool allowLegacyTC,
                            const img4_runtime_t *image4RT)
{
    /* Zero out everything */
    memset(runtime, 0, sizeof(*runtime));
    memset(mutableRT, 0, sizeof(*mutableRT));

    /* Set the mutable runtime pointer */
    runtime->mutableRT = mutableRT;

    /* Setup trust cache type permissions */
    runtime->allowSecondStaticTC = allowSecondStaticTC;
    runtime->allowEngineeringTC = allowEngineeringTC;
    runtime->allowLegacyTC = allowLegacyTC;

    /* Set the image4 runtime */
    runtime->image4RT = image4RT;
}

/**
 * Construct a trust cache object from some module bytes. The module is validated for
 * correctness before being returned.
 */
TCReturn_t
trustCacheConstructInvalid(TrustCache_t *trustCache,
                           const uint8_t *moduleAddr,
                           size_t moduleSize);

/**
 * Check the runtime for a trust cache which matches a particular UUID. Since we do
 * not allow trust caches with duplocate UUIDs, there can only ever be a single trust
 * cache with a particular UUID within the runtime.
 */
TCReturn_t
trustCacheCheckRuntimeForUUID(const TrustCacheRuntime_t *runtime,
                              const uint8_t checkUUID[kUUIDSize],
                              const TrustCache_t **trustCacheRet);

/**
 * Add a trust cache module directly to the runtime. This function is used to add modules which
 * don't need to be separately authenticated. Currently, the only trust cache types which can be
 * used with this function are static and engineering trust caches.
 *
 * If the system supports read-only segments, and the runtime is allocated within the read-only
 * segment, then this function needs to be called before the segment is enforced to be read-only.
 * For more information, please look at <TrustCache/Types.h>.
 */
TCReturn_t
trustCacheLoadModule(TrustCacheRuntime_t *runtime,
                     const TCType_t type,
                     TrustCache_t *trustCache,
                     const uintptr_t dataAddr,
                     const size_t dataSize);

/**
 * Load a trust cache onto the system. This function validates the trust cache for a proper
 * signature and adds it to the runtime.
 *
 * Both the payload and the manifest must be provided and they will be validated as image4
 * objects.
 */
TCReturn_t
trustCacheLoad(TrustCacheRuntime_t *runtime,
               TCType_t type,
               TrustCache_t *trustCache,
               const uintptr_t payloadAddr,
               const size_t payloadSize,
               const uintptr_t manifestAddr,
               const size_t manifestSize);

/**
 * Extract an image4 artifact from an image4 file or an image4 payload and extract the
 * trust cache module embedded within it. The module is validated for correctness
 * before being returned, however the image4 signature is not verified.
 *
 * The returned trust cache object is marked with an invalid type.
 */
TCReturn_t
trustCacheExtractModule(TrustCache_t *trustCache,
                        const uint8_t *dataAddr,
                        size_t dataSize);

/**
 * Query a  trust cache for a particular CDHash. The returned token can then be used to
 * query further attributes from the matched entry.
 */
TCReturn_t
trustCacheQuery(const TrustCacheRuntime_t *runtime,
                TCQueryType_t queryType,
                const uint8_t CDHash[kTCEntryHashSize],
                TrustCacheQueryToken_t *queryToken);

/**
 * Get the module bytes backng a trust cache object. The environment may have chosen
 * to allocate the module bytes within read-only memory, so the bytes returned may
 * not be mutable.
 */
TCReturn_t
trustCacheGetModule(const TrustCache_t *trustCache,
                    const uint8_t **moduleAddrRet,
                    size_t *moduleSizeRet);

/**
 * Get the UUID of the trust cache module represented by the wrapped trust cache object.
 */
TCReturn_t
trustCacheGetUUID(const TrustCache_t *trustCache,
                  uint8_t returnUUID[kUUIDSize]);

/**
 * Get the capabilities of a trust cache. This function can be used to query which fields a given
 * trust cache supports.
 *
 * The fields which are supported are based on the version of the trust cache module.
 */
TCReturn_t
trustCacheGetCapabilities(const TrustCache_t *trustCache,
                          TCCapabilities_t *capabilities);

/**
 * Acquire the trust cache type for a query token.
 */
TCReturn_t
trustCacheQueryGetTCType(const TrustCacheQueryToken_t *queryToken,
                         TCType_t *typeRet);

/**
 * Acquire the capabilities of the trust cache through a query token.
 */
TCReturn_t
trustCacheQueryGetCapabilities(const TrustCacheQueryToken_t *queryToken,
                               TCCapabilities_t *capabilities);

/**
 * Acquire the hash type for the CDHash through a query token.
 */
TCReturn_t
trustCacheQueryGetHashType(const TrustCacheQueryToken_t *queryToken,
                           uint8_t *hashTypeRet);

/**
 * Acquire the flags for a trust cache entry through a query token.
 */
TCReturn_t
trustCacheQueryGetFlags(const TrustCacheQueryToken_t *queryToken,
                        uint64_t *flagsRet);

/**
 * Acquire the constraint category for a trust cache entry through a query token.
 */
TCReturn_t
trustCacheQueryGetConstraintCategory(const TrustCacheQueryToken_t *queryToken,
                                     uint8_t *constraintCategoryRet);

__END_DECLS
#endif /* libTrustCache_API_h */
