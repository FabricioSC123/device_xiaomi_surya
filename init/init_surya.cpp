/*
   Copyright (C) 2019-2020 The LineageOS Project.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <string.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <android-base/properties.h>
#include <sys/sysinfo.h>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using std::string;

void property_override(char const prop[], char const value[], bool add = true)
{
    auto pi = (prop_info*) __system_property_find(prop);

    if (pi != nullptr)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_ro_build_prop(const string &source, const string &prop,
                       const string &value, bool product = false) {
    string prop_name;

    if (product)
        prop_name = "ro.product." + source + prop;
    else
        prop_name = "ro." + source + "build." + prop;

    property_override(prop_name.c_str(), value.c_str(), true);
}

void set_device_props(const string brand, const string device,
			const string model, const string name) {
    // list of partitions to override props
    string source_partitions[] = { "", "bootimage", "odm.", "product.",
                                   "system", "system_ext.", "vendor." };

    for (const string &source : source_partitions) {
        set_ro_build_prop(source, "brand", brand, true);
        set_ro_build_prop(source, "device", device, true);
        set_ro_build_prop(source, "product", device, false);
        set_ro_build_prop(source, "model", model, true);
        set_ro_build_prop(source, "name", name, true);
    }
}

void set_device_fp() {
    // list of partitions to override props
    string source_partitions[] = { "", "bootimage", "odm.", "product.",
                                   "system", "system_ext.", "vendor." };

    string fp = "Xiaomi/dipper/dipper:8.1.0/OPM1.171019.011/V9.5.5.0.OEAMIFA:user/release-keys";
    string desc = "dipper-user 8.1.0 OPM1.171019.011 V9.5.5.0.OEAMIFA release-keys";

    for (const string &source : source_partitions) {
        set_ro_build_prop(source, "fingerprint", fp, false);
        set_ro_build_prop(source, "description", desc, false);
    }
}


void load_dalvik_properties() {
    struct sysinfo sys;

    sysinfo(&sys);
    if (sys.totalram >= 5ull * 1024 * 1024 * 1024){
        // from - phone-xhdpi-6144-dalvik-heap.mk
        property_override("dalvik.vm.heapstartsize", "16m");
        property_override("dalvik.vm.heapgrowthlimit", "256m");
        property_override("dalvik.vm.heapsize", "512m");
        property_override("dalvik.vm.heaptargetutilization", "0.5");
        property_override("dalvik.vm.heapmaxfree", "32m");
    } else if (sys.totalram >= 7ull * 1024 * 1024 * 1024) {
        // 8GB
        property_override("dalvik.vm.heapstartsize", "24m");
        property_override("dalvik.vm.heapgrowthlimit", "256m");
        property_override("dalvik.vm.heapsize", "512m");
        property_override("dalvik.vm.heaptargetutilization", "0.46");
        property_override("dalvik.vm.heapmaxfree", "48m");
    }

    property_override("dalvik.vm.heapminfree", "8m");
}

void vendor_load_properties()
{
    /*
     * Detect device and configure properties
     */
    if (GetProperty("ro.boot.hwname", "") == "karna") { // POCO X3 (India)
        set_device_props("POCO", "karna", "POCO X3", "karna_in");
        property_override("ro.product.marketname", "POCO X3");
        property_override("ro.product.mod_device", "surya_in_global");
        property_override("ro.product.model", "POCO X3");
        property_override("ro.product.system.model", "qssi system image for arm64");
        property_override("ro.product.vendor.model", "POCO X3");
        property_override("ro.product.product.model", "qssi system image for arm64");
        property_override("ro.product.odm.model", "POCO X3");
        property_override("ro.product.system_ext.model", "qssi system image for arm64");
        property_override("ro.product.build.fingerprint", "qti/qssi/qssi:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.system_ext.build.fingerprint", "qti/qssi/qssi:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.system.build.fingerprint", "qti/qssi/qssi:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.vendor.build.fingerprint", "POCO/karna_global/karna:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.bootimage.build.fingerprint", "POCO/karna_global/karna:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.odm.build.fingerprint", "POCO/karna_global/karna:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.build.fingerprint", "POCO/karna_global/karna:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.build.description", "karna_global-user 11 RKQ1.200826.002 V12.5.1.0.RJGMIXM release-keys");
    } else { // POCO X3 NFC
        set_device_props("POCO", "surya", "POCO X3 NFC", "surya_global");
        property_override("ro.product.marketname", "POCO X3 NFC");
        property_override("ro.product.mod_device", "surya_global");
        property_override("ro.product.model", "POCO X3 NFC");
        property_override("ro.product.system.model", "qssi system image for arm64");
        property_override("ro.product.vendor.model", "POCO X3 NFC");
        property_override("ro.product.product.model", "qssi system image for arm64");
        property_override("ro.product.odm.model", "POCO X3 NFC");
        property_override("ro.product.system_ext.model", "qssi system image for arm64");
        property_override("ro.product.build.fingerprint", "qti/qssi/qssi:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.system_ext.build.fingerprint", "qti/qssi/qssi:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.system.build.fingerprint", "qti/qssi/qssi:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.vendor.build.fingerprint", "POCO/surya_global/surya:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.bootimage.build.fingerprint", "POCO/surya_global/surya:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.odm.build.fingerprint", "POCO/surya_global/surya:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.build.fingerprint", "POCO/surya_global/surya:11/RKQ1.200826.002/V12.5.1.0.RJGMIXM:user/release-keys");
        property_override("ro.build.description", "surya_global-user 11 RKQ1.200826.002 V12.5.1.0.RJGMIXM release-keys");
    }

    load_dalvik_properties();
    //Safetynet workarounds
    set_device_fp();
    property_override("ro.oem_unlock_supported", "0");
    property_override("ro.boot.verifiedbootstate", "green");

    // Set hardware revision
    property_override("ro.boot.hardware.revision", GetProperty("ro.boot.hwversion", "").c_str());
}
