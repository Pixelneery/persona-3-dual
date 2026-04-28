#include "controllers/AnimationController.h"

AnimationController::AnimationController() {}

void AnimationController::loadModel(const vector<AnimNode>& nodes, const vector<Animation>& anims) {
    modelNodes = nodes;
    animations = anims;
    
    // auto-link children and find root nodes
    rootNodes.clear();
    for (int i = 0; i < (int)modelNodes.size(); i++) {
        modelNodes[i].children.clear(); // Safety clear
    }

    for (int i = 0; i < (int)modelNodes.size(); i++) {
        if (modelNodes[i].parentId == -1) {
            rootNodes.push_back(i);
        } else if (modelNodes[i].parentId >= 0 && modelNodes[i].parentId < (int)modelNodes.size()) {
            // Register as child to its parent
            modelNodes[modelNodes[i].parentId].children.push_back(i);
        }
    }
}

void AnimationController::set(int animIndex, bool loop) {
    if (animIndex < 0 || animIndex >= (int)animations.size()) return;
    currentAnimIndex = animIndex;
    currentFrame = 0;
    isFinishing = false;
    isLooping = loop;
}

void AnimationController::play() {
    if (currentAnimIndex == -1) return;
    isPlaying = true;
    isFinishing = false;
    
    // Only reset the frame if the animation is fully stopped or finished.
    // This allows play() to act as "resume" if it was hard-paused mid-animation.
    if (currentFrame >= animations[currentAnimIndex].duration - 1) {
        currentFrame = 0;
    }
}

void AnimationController::stop() {
    isPlaying = false;
    isFinishing = false;
    currentFrame = 0;
}

void AnimationController::pause() {
    // Flags the animation to stop looping once it hits its maximum duration
    isFinishing = true;
}

void AnimationController::update() {
    if (!isPlaying || currentAnimIndex == -1) return;

    currentFrame++;
    
    if (currentFrame >= animations[currentAnimIndex].duration) {
        if (isFinishing || !isLooping) {
            isPlaying = false;
            isFinishing = false;
            
            // If it's a 1-shot animation, hold on the final frame.
            // If it was gracefully paused while looping, snap cleanly to 0.
            if (!isLooping) {
                currentFrame = animations[currentAnimIndex].duration - 1;
            } else {
                currentFrame = 0; 
            }
        } else {
            currentFrame = 0; // Loop seamlessly
        }
    }
}

Keyframe AnimationController::getInterpolatedFrame(const AnimTrack& track, int time) {
    if (track.frames.empty()) return {0, 0,0,0, 0,0,0};
    if (track.frames.size() == 1) return track.frames[0];

    Keyframe k1 = track.frames[0];
    Keyframe k2 = track.frames[track.frames.size() - 1];

    for (size_t i = 0; i < track.frames.size() - 1; i++) {
        if (time >= track.frames[i].time && time < track.frames[i+1].time) {
            k1 = track.frames[i];
            k2 = track.frames[i+1];
            break;
        }
    }

    if (k1.time == k2.time) return k1;

    int range = k2.time - k1.time;
    int progress = time - k1.time;
    int t = (progress << 12) / range; // fixed point lerp

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
        Keyframe current = getInterpolatedFrame(track, currentFrame);

        // move to the joint's pivot point + the animation's position offset
        glTranslatef32(node.pivotX + current.posX, node.pivotY + current.posY, node.pivotZ + current.posZ);
        
        // rotate around the pivot point
        glRotateZi(current.rotZ);
        glRotateYi(current.rotY);
        glRotateXi(current.rotX);
        
        // move back from the pivot point so the geometry aligns
        glTranslatef32(-node.pivotX, -node.pivotY, -node.pivotZ);
    }

    if (node.displayListSize > 0) {
        glCallList(node.displayList);
    }

    for (int childId : node.children) {
        renderNode(childId);
    }

    glPopMatrix(1);
}