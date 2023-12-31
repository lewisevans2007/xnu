//
//  CoreEntitlementsPriv.h
//  CoreEntitlements
//

#ifndef CORE_ENTITLEMENTS_PRIV_H
#define CORE_ENTITLEMENTS_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <CoreEntitlements/CoreEntitlements.h>
#include <CoreEntitlements/EntitlementsPriv.h>

#define CE_BRIDGE_STRUCT_VERSION 3
#define CCDER_ENTITLEMENTS (CCDER_SEQUENCE | CCDER_CONSTRUCTED | CCDER_APPLICATION)

__ptrcheck_abi_assume_single();

/*!
 * @typedef coreentitlements_t
 * Wraps up all the CoreEntitlements functions into a nice bundle to be used in the kernel
 */
typedef struct {
    uint64_t version;
    typeof(&CEAcquireUnmanagedContext) AcquireUnmanagedContext;
    typeof(&CEValidate) Validate;
    typeof(&CEContextQuery) ContextQuery;
    typeof(&CEConjureContextFromDER) ConjureContextFromDER;
    
    typeof(&der_vm_context_create) der_vm_context_create;
    typeof(&der_vm_execute) der_vm_execute;
    typeof(&der_vm_iterate) der_vm_iterate;
    typeof(&der_vm_context_is_valid) der_vm_context_is_valid;
    typeof(&der_vm_CEType_from_context) der_vm_CEType_from_context;
    typeof(&der_vm_integer_from_context) der_vm_integer_from_context;
    typeof(&der_vm_string_from_context) der_vm_string_from_context;
    typeof(&der_vm_bool_from_context) der_vm_bool_from_context;
    
    typeof(kCENoError) kNoError;
    typeof(kCEAPIMisuse) kAPIMisuse;
    typeof(kCEInvalidArgument) kInvalidArgument;
    typeof(kCEAllocationFailed) kAllocationFailed;
    typeof(kCEMalformedEntitlements) kMalformedEntitlements;
    typeof(kCEQueryCannotBeSatisfied) kQueryCannotBeSatisfied;
#if CE_ACCELERATION_SUPPORTED
    typeof(kCENotEligibleForAcceleration) kNotEligibleForAcceleration;
#endif

    typeof(&CEGetErrorString) GetErrorString;
    
    typeof(&der_vm_buffer_from_context) der_vm_buffer_from_context;
    typeof(&CEContextIsSubset) CEContextIsSubset;
    
#if CE_ACCELERATION_SUPPORTED
    typeof(&CEIndexSizeForContext) IndexSizeForContext;
    typeof(&CEBuildIndexForContext) BuildIndexForContext;
    typeof(&CEFreeIndexForContext) FreeIndexForContext;
    typeof(&CEContextIsAccelerated) ContextIsAccelerated;
#endif
} coreentitlements_t;

#ifdef __BLOCKS__
typedef bool (^iteration_trampoline_t)(der_vm_iteration_context ctx);
bool der_vm_block_trampoline(der_vm_iteration_context ctx);
#endif

/*
 These are private for now, they're essentially inverse functions for CESerialize.
 
 */
CEError_t CESizeDeserialization(CEQueryContext_t ctx, size_t* requiredElements);
CEError_t CEDeserialize(CEQueryContext_t ctx, CESerializedElement_t *__counted_by(elementsLength) elements, size_t elementsLength);

#ifdef __cplusplus
}
#endif

#endif
