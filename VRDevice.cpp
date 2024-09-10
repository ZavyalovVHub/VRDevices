#include "VRDevice.h"

#include <QDebug>
#include <cmath>

using namespace std;
using namespace vr;

void VRDevice::VrInit() {
    if (!vr) {
        vr = VR_Init(&eError, VRApplication_Overlay);
    }
}

bool VRDevice::FindDevice() {
    if (!vr) {
        return false;
    }
    auto lastErr = TrackedProp_Success;

    for (int i = 0; i < k_unMaxTrackedDeviceCount; i++) {
        if (vr->GetInt32TrackedDeviceProperty(i, Prop_DeviceClass_Int32, &lastErr)) {
            int device_class = vr->GetInt32TrackedDeviceProperty(i, Prop_DeviceClass_Int32, &lastErr);
            switch (device_class) {
            case 1: //HMD
                if (HMD_index.indexOf(i) == -1) {
                    HMD_index.append(i);
                }
                break;
            case 2: //controller

                break;
            case 3: //tracker
                if (trackers_index.indexOf(i) == -1) {
                    trackers_index.append(i);
                    trackers_struct.resize(trackers_index.size());
                }
                break;
            case 4: //base station
                if (BS_index.indexOf(i) == -1) {
                    BS_index.append(i);
                    BS_state.resize(BS_index.size());
                }
                break;
            }
        }
    }
    return true;
}

QVector<VRDevice::StateTR> VRDevice::GetTrackerInfo() {
    auto lastErr = TrackedProp_Success;
    VREvent_t pEvent;

    if (vr) {
        for (int i = 0; i < trackers_index.size(); i++) {

            if (trackers_struct.size()) {
                trackers_struct[i].batteryPercent = round((vr->GetFloatTrackedDeviceProperty(trackers_index[i], Prop_DeviceBatteryPercentage_Float, &lastErr)) * 100);

                char serial_NUM[k_unMaxPropertyStringSize];
                vr->GetStringTrackedDeviceProperty(trackers_index[i], Prop_SerialNumber_String, serial_NUM, k_unMaxPropertyStringSize, &lastErr);
                trackers_struct[i].serial = serial_NUM;

                vr->PollNextEventWithPose(TrackingUniverseSeated, &pEvent, sizeof(pEvent), device_pose_T);
                vr->GetDeviceToAbsoluteTrackingPose(TrackingUniverseSeated, 0, device_pose_T, k_unMaxTrackedDeviceCount);
                if (device_pose_T[trackers_index[i]].bDeviceIsConnected) {
                    if (device_pose_T[trackers_index[i]].bPoseIsValid) {
                        trackers_struct[i].activity = 1;
                    } else {
                        trackers_struct[i].activity = 2;
                    }
                } else {
                    trackers_struct[i].activity = 0;
                }
            }
        }
    }
    return trackers_struct;
}

int VRDevice::GetHelmetInfo() {
    if (vr) {
        vr->GetDeviceToAbsoluteTrackingPose(TrackingUniverseSeated, 0, device_pose_H, k_unMaxTrackedDeviceCount);
        for (int i = 0; i < HMD_index.size(); i++) {
            if (device_pose_H[HMD_index[0]].bDeviceIsConnected) {
                if (device_pose_H[HMD_index[0]].bPoseIsValid) {
                    HMD_state = 1; //helmet work
                } else {
                    HMD_state = 2; //helmet lost
                }
            } else {
                HMD_state = 0; //helmet off
            }
        }
    }

    return HMD_state;
}

QVector<int> VRDevice::GetBasestationInfo() {
    if (vr) {
        vr->GetDeviceToAbsoluteTrackingPose(TrackingUniverseSeated, 0, device_pose_B, k_unMaxTrackedDeviceCount);
        for (int i = 0; i < BS_index.size(); i++) {
            if (BS_state.size()) {
                if (device_pose_B[BS_index[i]].bDeviceIsConnected) {
                    BS_state[i] = 1;
                } else {
                    BS_state[i] = 0;
                }
            }
        }
    }

    return BS_state;
}

VRDevice::~VRDevice() {
    if (vr) {
        vr::VR_Shutdown();
        vr = nullptr;
        printf("VR_Shutdown \n");
    }
}






