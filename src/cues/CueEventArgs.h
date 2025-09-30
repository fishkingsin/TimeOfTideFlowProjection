//  CueEventArgs.h
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 23/9/2025.
//

#pragma once
#include <string>
#include <map>

enum class CueType {
    SceneChange,
    ConfigUpdate,
    Unknown
};

class CueEventArgs {
public:
    CueType cueType = CueType::Unknown;
    int sceneId = -1;
    std::map<std::string, float> parameters;

    CueEventArgs() = default;
    CueEventArgs(CueType type, int id, const std::map<std::string, float>& params)
        : cueType(type), sceneId(id), parameters(params) {}
};
