#ifndef VRDEVICE_H
#define VRDEVICE_H

#include <QString>
#include <QVector>
#include "openvr.h"

class VRDevice {
public:
    struct StateTR {
        int activity = 0;
        int batteryPercent = 0;
        QString serial;
    };

    VRDevice() = default;
    ~VRDevice();
    void VrInit();
    bool FindDevice();
    QVector<StateTR> GetTrackerInfo();
    int GetHelmetInfo();
    QVector<int> GetBasestationInfo();

private:
    //init vr system
    vr::EVRInitError eError = vr::VRInitError_None;
    vr::IVRSystem *vr = nullptr;

    //helmet state
    vr::TrackedDevicePose_t device_pose_H[vr::k_unMaxTrackedDeviceCount];
    QVector<uint> HMD_index; //array of HMD indices
    int HMD_state = 0;

    //basestation state
    vr::TrackedDevicePose_t device_pose_B[vr::k_unMaxTrackedDeviceCount];
    uint len = 0;
    uint devices[vr::k_unMaxTrackedDeviceCount];
    QVector<uint> BS_index; //array of BS indices
    QVector<int> BS_state;

    //trackers state
    QVector<uint> trackers_index; //array of trackers indices
    vr::TrackedDevicePose_t device_pose_T[vr::k_unMaxTrackedDeviceCount];
    QVector<StateTR> trackers_struct;

};

#endif // VRDEVICE_H
