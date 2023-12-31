/*
 * Copyright (c) 2000-2006 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 *
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
/* IOOffset.m created by rsulack on Wed 17-Sep-1997 */

#define IOKIT_ENABLE_SHARED_PTR

#include <sys/cdefs.h>

#include <libkern/c++/OSNumber.h>
#include <libkern/c++/OSString.h>
#include <libkern/c++/OSSerialize.h>
#include <libkern/c++/OSSharedPtr.h>
#include <libkern/c++/OSLib.h>

#define sizeMask (~0ULL >> (64 - size))

#define super OSObject

OSDefineMetaClassAndStructorsWithZone(OSNumber, OSObject,
    (zone_create_flags_t) (ZC_CACHING | ZC_ZFREE_CLEARMEM))

OSMetaClassDefineReservedUnused(OSNumber, 0);
OSMetaClassDefineReservedUnused(OSNumber, 1);
OSMetaClassDefineReservedUnused(OSNumber, 2);
OSMetaClassDefineReservedUnused(OSNumber, 3);
OSMetaClassDefineReservedUnused(OSNumber, 4);
OSMetaClassDefineReservedUnused(OSNumber, 5);
OSMetaClassDefineReservedUnused(OSNumber, 6);
OSMetaClassDefineReservedUnused(OSNumber, 7);

bool
OSNumber::init(unsigned long long inValue, unsigned int newNumberOfBits)
{
	if (!super::init()) {
		return false;
	}
	if (newNumberOfBits > 64) {
		return false;
	}

	size = newNumberOfBits;
	value = (inValue & sizeMask);

	return true;
}

bool
OSNumber::init(const char *newValue, unsigned int newNumberOfBits)
{
	return init((unsigned long long)strtoul(newValue, NULL, 0), newNumberOfBits);
}

void
OSNumber::free()
{
	super::free();
}

OSSharedPtr<OSNumber>
OSNumber::withNumber(unsigned long long value,
    unsigned int newNumberOfBits)
{
	OSSharedPtr<OSNumber> me = OSMakeShared<OSNumber>();

	if (me && !me->init(value, newNumberOfBits)) {
		return nullptr;
	}

	return me;
}

OSSharedPtr<OSNumber>
OSNumber::withNumber(const char *value, unsigned int newNumberOfBits)
{
	OSSharedPtr<OSNumber> me = OSMakeShared<OSNumber>();

	if (me && !me->init(value, newNumberOfBits)) {
		return nullptr;
	}

	return me;
}

OSSharedPtr<OSNumber>
OSNumber::withDouble(
	double             value)
{
	OSSharedPtr<OSNumber> me = OSMakeShared<OSNumber>();

	if (me && !me->OSObject::init()) {
		return nullptr;
	}
	me->size = 63;
	me->fpValue = value;

	return me;
}

OSSharedPtr<OSNumber>
OSNumber::withFloat(
	float             value)
{
	OSSharedPtr<OSNumber> me = OSMakeShared<OSNumber>();

	if (me && !me->OSObject::init()) {
		return nullptr;
	}
	me->size = 31;
	me->fpValue = (double) value;

	return me;
}

double
OSNumber::doubleValue() const
{
	if ((size != 63) && (size != 31)) {
		return (double) value;
	}
	return fpValue;
}

float
OSNumber::floatValue() const
{
	if ((size != 63) && (size != 31)) {
		return (float) value;
	}
	return (float) fpValue;
}

unsigned int
OSNumber::numberOfBits() const
{
	return size;
}

unsigned int
OSNumber::numberOfBytes() const
{
	return (size + 7) / 8;
}


unsigned char
OSNumber::unsigned8BitValue() const
{
	if ((size == 63) || (size == 31)) {
		return (unsigned char) fpValue;
	}
	return (unsigned char) value;
}

unsigned short
OSNumber::unsigned16BitValue() const
{
	if ((size == 63) || (size == 31)) {
		return (unsigned short) fpValue;
	}
	return (unsigned short) value;
}

unsigned int
OSNumber::unsigned32BitValue() const
{
	if ((size == 63) || (size == 31)) {
		return (unsigned int) fpValue;
	}
	return (unsigned int) value;
}

unsigned long long
OSNumber::unsigned64BitValue() const
{
	if ((size == 63) || (size == 31)) {
		return (unsigned long long) fpValue;
	}
	return value;
}

void
OSNumber::addValue(signed long long inValue)
{
	if ((size == 63) || (size == 31)) {
		fpValue += inValue;
	} else {
		value = ((value + inValue) & sizeMask);
	}
}

void
OSNumber::setValue(unsigned long long inValue)
{
	if ((size == 63) || (size == 31)) {
		fpValue = (double) inValue;
	} else {
		value = (inValue & sizeMask);
	}
}

bool
OSNumber::isEqualTo(const OSNumber *integer) const
{
	return unsigned64BitValue() == integer->unsigned64BitValue();
}

bool
OSNumber::isEqualTo(const OSMetaClassBase *obj) const
{
	OSNumber *  offset;
	if ((offset = OSDynamicCast(OSNumber, obj))) {
		return isEqualTo(offset);
	} else {
		return false;
	}
}

bool
OSNumber::serialize(OSSerialize *s) const
{
	char temp[32];

	if (s->previouslySerialized(this)) {
		return true;
	}

	snprintf(temp, sizeof(temp), "integer size=\"%d\"", size);
	if (!s->addXMLStartTag(this, temp)) {
		return false;
	}

	//XXX    sprintf(temp, "0x%qx", value);
	if ((value >> 32)) {
		snprintf(temp, sizeof(temp), "0x%lx%08lx", (unsigned long)(value >> 32),
		    (unsigned long)(value & 0xFFFFFFFF));
	} else {
		snprintf(temp, sizeof(temp), "0x%lx", (unsigned long)value);
	}
	if (!s->addString(temp)) {
		return false;
	}

	return s->addXMLEndTag("integer");
}
