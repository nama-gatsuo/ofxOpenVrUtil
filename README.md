# ofxOpenVrUtil
openFrameworks addon for bridging [OpenVR](https://github.com/ValveSoftware/openvr). OpenVR is the open source API to access VR system via [SteamVR](https://store.steampowered.com/steamvr). Main support target is HTC VIVE (I was using VIVE PRO for developping) but it might be compatible to Oculus Rift S, Valve Index or other VR system which can use SteamVR platform for just displaying a scene. But for Oculus series, the vendor (facebook) published native SDK which would run faster than access via OpenVR.

## Setup
* OpenVR is a submodule of this repo, so we have to `git submodule`

```
> cd of\addon
> git clone https://github.com/nama-gatsuo/ofxOpenVrUtil
> git submodule init
> git submodule update

```

* You can then use the OF Project Generator to generate projects with the appropriate headers and libraries included.
