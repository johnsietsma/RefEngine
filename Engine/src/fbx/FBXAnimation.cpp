#include "FBXAnimation.h"

#include "FBXNode.h"

#include <glm/gtx/transform.hpp>

void FBXSkeleton::evaluate(const FBXAnimation* a_animation, float a_time, bool a_loop, float a_FPS)
{
    if (a_animation != nullptr)
    {
        // determine frame we're on
        int totalFrames = a_animation->m_endFrame - a_animation->m_startFrame;
        float animDuration = totalFrames / a_FPS;

        // get time through frame
        float frameTime = 0;
        if (a_loop)
            frameTime = glm::max(glm::mod(a_time,animDuration),0.0f);
        else
            frameTime = glm::min(glm::max(a_time,0.0f),animDuration);

        const FBXTrack* track = nullptr;
        const FBXKeyFrame* start = nullptr;
        const FBXKeyFrame* end = nullptr;

        for ( unsigned int i = 0 ; i < a_animation->m_trackCount ; ++i )
        {
            track = &(a_animation->m_tracks[i]);

            start = nullptr;
            end = nullptr;
            float startTime, endTime;

            // determine the two keyframes we're between
            for ( unsigned int j = 0 ; j < track->m_keyframeCount - 1 ; ++j )
            {
                startTime = (track->m_keyframes[j].m_key - a_animation->m_startFrame) / a_FPS;
                endTime = (track->m_keyframes[j + 1].m_key - a_animation->m_startFrame) / a_FPS;

                if (startTime <= frameTime &&
                    endTime >= frameTime)
                {
                    // found?
                    start = &(track->m_keyframes[j]);
                    end = &(track->m_keyframes[j+1]);
                    break;
                }
            }

            // interpolate between them
            if (start != nullptr &&
                end != nullptr)
            {
                float fScale = glm::max(0.0f,glm::min(1.0f,(frameTime - startTime) / (endTime - startTime)));

                // translation
                glm::vec3 T = glm::mix(start->m_translation,end->m_translation,fScale);

                // scale
                glm::vec3 S = glm::mix(start->m_scale,end->m_scale,fScale);

                // rotation (quaternion slerp)
                glm::quat R = glm::normalize(glm::slerp(start->m_rotation,end->m_rotation,fScale));

                // build matrix
                glm::mat4 mRot = glm::mat4_cast( R );
                glm::mat4 mScale = glm::scale( S );
                glm::mat4 mTranslate = glm::translate( T );
                m_nodes[ track->m_boneIndex ]->m_localTransform = mTranslate * mScale * mRot;
            }
        }
    }
}

void FBXSkeleton::updateBones()
{
    // update bones
    for ( int i = 0 ; i < (int)m_boneCount ; ++i )
    {
        if ( m_parentIndex[i] == -1 )
            m_bones[ i ] = m_nodes[ i ]->m_localTransform;
        else
            m_bones[i] = m_bones[ m_parentIndex[ i ] ] * m_nodes[ i ]->m_localTransform;
    }
    // combine bind pose
    for ( int i = 0 ; i < (int)m_boneCount ; ++i )
    {
        m_bones[ i ] = m_bones[ i ] * m_bindPoses[ i ];
    }
}
