#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>

class FBXNode;

// A single frame for a bone in an animation
class FBXKeyFrame
{
public:

    FBXKeyFrame();
    ~FBXKeyFrame();

    unsigned int    m_key;
    glm::quat       m_rotation;
    glm::vec3       m_translation;
    glm::vec3       m_scale;
};

// A collection of frames for a single bone in an animation
class FBXTrack
{
public:

    FBXTrack();
    ~FBXTrack();

    unsigned int    m_boneIndex;
    unsigned int    m_keyframeCount;
    FBXKeyFrame*    m_keyframes;
};

// An animation that contains a collection of animated bone tracks
class FBXAnimation
{
public:

    FBXAnimation();
    ~FBXAnimation();

    // creates a deep-copy of this animation (caller takes ownership of returned data)
    FBXAnimation*   clone() const;

    unsigned int    totalFrames() const;
    float           totalTime(float a_fps = 24.0f) const;

    std::string     m_name;
    unsigned int    m_startFrame;
    unsigned int    m_endFrame;
    unsigned int    m_trackCount;
    FBXTrack*       m_tracks;
};

// A hierarchy of bones that can be animated
class FBXSkeleton
{
public:

    FBXSkeleton();
    ~FBXSkeleton();

    void            evaluate(const FBXAnimation* a_animation, float a_time, bool a_loop = true, float a_fps = 24.0f);
    void            updateBones();

    unsigned int    m_boneCount;
    FBXNode**       m_nodes;
    int*            m_parentIndex;

    glm::mat4*      m_bones;    // ready for use in skinning! (bind pose combined)
    glm::mat4*      m_bindPoses;

    void*           m_userData;
};

inline FBXKeyFrame::FBXKeyFrame()
    : m_key(0),
    m_rotation(0, 0, 0, 1),
    m_translation(0, 0, 0),
    m_scale(1, 1, 1)
{

}

inline FBXKeyFrame::~FBXKeyFrame()
{

}

inline FBXTrack::FBXTrack()
    : m_boneIndex(0),
    m_keyframeCount(0),
    m_keyframes(nullptr)
{

}

inline FBXTrack::~FBXTrack()
{
    delete[] m_keyframes;
}

inline FBXAnimation::FBXAnimation()
    : m_startFrame(0xffffffff),
    m_endFrame(0),
    m_trackCount(0),
    m_tracks(nullptr)
{

}

inline FBXAnimation::~FBXAnimation()
{
    delete[] m_tracks;
}

inline unsigned int FBXAnimation::totalFrames() const
{
    return m_endFrame - m_startFrame;
}

inline float FBXAnimation::totalTime(float a_fps /* = 24.0f */) const
{
    return (m_endFrame - m_startFrame) / a_fps;
}

inline FBXSkeleton::FBXSkeleton()
    : m_boneCount(0),
    m_nodes(nullptr),
    m_parentIndex(nullptr),
    m_bones(nullptr),
    m_bindPoses(nullptr),
    m_userData(nullptr)
{

}

inline FBXSkeleton::~FBXSkeleton()
{
    delete[] m_parentIndex;
    delete[] m_nodes;
    free(m_bones);
    free(m_bindPoses);
}