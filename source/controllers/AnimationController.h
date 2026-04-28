#pragma once
#include <nds.h>
#include <vector>
#include <string>

using namespace std;

// Represents a single frame of animation for a specific node
struct Keyframe {
    int time;            // Frame time (e.g., 0, 10, 20)
    s16 rotX, rotY, rotZ; // Angles (using libnds 0-8191 format)
    v16 posX, posY, posZ; // Fixed point translation
};

// Represents an animation track for a single node
struct AnimTrack {
    vector<Keyframe> frames;
};

// Represents a full animation (e.g., "walk", "attack")
struct Animation {
    string name;
    int duration; // Total frames in this animation
    vector<AnimTrack> nodeTracks; // One track per node in the model
};

// Represents a body part (e.g., Torso, Arm)
struct AnimNode {
    int id;
    int parentId; // -1 if root
    u32* displayList; 
    u32 displayListSize; 
    vector<int> children; // Auto-populated by the controller
    v16 pivotX, pivotY, pivotZ; // <--- ADD THESE 3
};

class AnimationController {
    public:
        AnimationController();
        
        // Loads the auto-generated data
        void loadModel(const vector<AnimNode>& nodes, const vector<Animation>& anims);
        
        // Play an animation by index (0, 1, 2...)
        void play(int animIndex, bool loop = true);
        
        // Advance the animation by 1 frame (call once per VBlank)
        void update();
        
        // Submits the matrices and geometry to the GX Engine
        void render();

        bool isPlaying() const { return active; }
        int getCurrentAnimIndex() const { return currentAnimIndex; }

    private:
        void renderNode(int nodeId);
        Keyframe getInterpolatedFrame(const AnimTrack& track, int currentTime);

        vector<AnimNode> modelNodes;
        vector<Animation> animations;
        
        int currentAnimIndex = -1;
        int currentFrame = 0;
        bool isLooping = false;
        bool active = false;
        
        vector<int> rootNodes; 
};