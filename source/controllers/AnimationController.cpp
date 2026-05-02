#include "controllers/AnimationController.h"
#include <stdio.h>
#include <string.h>

AnimationController::AnimationController() {}

bool AnimationController::loadModel(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) return false;

    char magic[4];
    fread(magic, 1, 4, file);
    if (strncmp(magic, "MDL1", 4) != 0) {
        fclose(file);
        return false;
    }

    u32 numNodes, numAnims;
    fread(&numNodes, sizeof(u32), 1, file);
    fread(&numAnims, sizeof(u32), 1, file);

    modelNodes.resize(numNodes);
    rootNodes.clear();

    for (u32 i = 0; i < numNodes; i++) {
        AnimNode& node = modelNodes[i];
        node.id = i;
        node.children.clear();

        s32 pid, px, py, pz;
        u32 dlSize;
        
        fread(&pid, sizeof(s32), 1, file);
        fread(&px, sizeof(s32), 1, file);
        fread(&py, sizeof(s32), 1, file);
        fread(&pz, sizeof(s32), 1, file);
        fread(&dlSize, sizeof(u32), 1, file);

        node.parentId = pid;
        node.pivotX = px;
        node.pivotY = py;
        node.pivotZ = pz;
        node.displayListSize = dlSize;

        node.displayList.resize(dlSize + 1);
        node.displayList[0] = dlSize;

        if (dlSize > 0) {
            fread(&node.displayList[1], sizeof(u32), dlSize, file);
        }

        if (pid == -1) {
            rootNodes.push_back(i);
        } else if (pid >= 0 && pid < (int)numNodes) {
            modelNodes[pid].children.push_back(i);
        }
    }

    animations.resize(numAnims);
    for (u32 i = 0; i < numAnims; i++) {
        Animation& anim = animations[i];
        
        // skip the 32-byte string name saved by Python
        fseek(file, 32, SEEK_CUR);
        
        fread(&anim.duration, sizeof(u32), 1, file);

        anim.nodeTracks.resize(numNodes);
        for (u32 n = 0; n < numNodes; n++) {
            AnimTrack& track = anim.nodeTracks[n];
            u32 numFrames;
            fread(&numFrames, sizeof(u32), 1, file);
            
            track.frames.resize(numFrames);
            if (numFrames > 0) {
                fread(track.frames.data(), sizeof(Keyframe), numFrames, file);
            }
        }
    }

    fclose(file);
    trackIndices.assign(modelNodes.size(), 0);
    return true;
}

void AnimationController::set(int animIndex, bool loop) {
    if (animIndex < 0 || animIndex >= (int)animations.size()) return;
    currentAnimIndex = animIndex;
    currentFrame = 0;
    isFinishing = false;
    isLooping = loop;
    fill(trackIndices.begin(), trackIndices.end(), 0);
}

void AnimationController::play() {
    if (currentAnimIndex == -1) return;
    isPlaying = true;
    isFinishing = false;
    if (currentFrame >= animations[currentAnimIndex].duration - 1) {
        currentFrame = 0;
        fill(trackIndices.begin(), trackIndices.end(), 0);
    }
}

void AnimationController::stop() {
    isPlaying = false;
    isFinishing = false;
    currentFrame = 0;
    fill(trackIndices.begin(), trackIndices.end(), 0);
}

void AnimationController::pause() {
    isFinishing = true;
}

void AnimationController::update() {
    if (!isPlaying || currentAnimIndex == -1) return;

    currentFrame++;
    
    if (currentFrame >= animations[currentAnimIndex].duration) {
        if (isFinishing || !isLooping) {
            isPlaying = false;
            isFinishing = false;
            if (!isLooping) {
                currentFrame = animations[currentAnimIndex].duration - 1;
            } else {
                currentFrame = 0; 
                fill(trackIndices.begin(), trackIndices.end(), 0);
            }
        } else {
            currentFrame = 0; 
            fill(trackIndices.begin(), trackIndices.end(), 0);
        }
    }
}

Keyframe AnimationController::getInterpolatedFrame(const AnimTrack& track, int time, int nodeId) {
    if (track.frames.empty()) return {0, 0,0,0, 0,0,0};
    if (track.frames.size() == 1) return track.frames[0];

    int& cacheIdx = trackIndices[nodeId];
    
    if (cacheIdx >= (int)track.frames.size() - 1 || time < track.frames[cacheIdx].time) {
        cacheIdx = 0; 
    }

    while (cacheIdx < (int)track.frames.size() - 1 && time >= track.frames[cacheIdx + 1].time) {
        cacheIdx++;
    }

    Keyframe k1 = track.frames[cacheIdx];
    if (cacheIdx >= (int)track.frames.size() - 1) return k1; 
    
    Keyframe k2 = track.frames[cacheIdx + 1];

    int range = k2.time - k1.time;
    if (range == 0) return k1; 

    int progress = time - k1.time;
    if (progress == 0) return k1; 

    int t = (progress << 12) / range; 

    Keyframe result;
    result.time = time;
    result.posX = k1.posX + (((k2.posX - k1.posX) * t) >> 12);
    result.posY = k1.posY + (((k2.posY - k1.posY) * t) >> 12);
    result.posZ = k1.posZ + (((k2.posZ - k1.posZ) * t) >> 12);
    result.rotX = k1.rotX + (((k2.rotX - k1.rotX) * t) >> 12);
    result.rotY = k1.rotY + (((k2.rotY - k1.rotY) * t) >> 12);
    result.rotZ = k1.rotZ + (((k2.rotZ - k1.rotZ) * t) >> 12);

    return result;
}

void AnimationController::render() {
    if (modelNodes.empty()) return;

    glMatrixMode(GL_MODELVIEW);
    for (int rootId : rootNodes) {
        renderNode(rootId);
    }
}

void AnimationController::renderNode(int nodeId) {
    AnimNode& node = modelNodes[nodeId];
    glPushMatrix();

    if (currentAnimIndex != -1) {
        AnimTrack& track = animations[currentAnimIndex].nodeTracks[nodeId];
        Keyframe current = getInterpolatedFrame(track, currentFrame, nodeId);

        glTranslatef32(node.pivotX + current.posX, node.pivotY + current.posY, node.pivotZ + current.posZ);
        glRotateZi(current.rotZ);
        glRotateYi(current.rotY);
        glRotateXi(current.rotX);
        glTranslatef32(-node.pivotX, -node.pivotY, -node.pivotZ);
    }

    if (node.displayListSize > 0) {
        glCallList(node.displayList.data());
    }

    for (int childId : node.children) {
        renderNode(childId);
    }

    glPopMatrix(1);
}