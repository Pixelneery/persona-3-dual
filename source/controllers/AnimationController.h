#pragma once
#include <nds.h>
#include <vector>
#include <string>

using namespace std;

// represents a single frame of animation for a specific node
struct Keyframe {
    int time;             // frame time (e.g., 0, 10, 20)
    s16 rotX, rotY, rotZ; // angles (using libnds 0-8191 format)
    v16 posX, posY, posZ; // fixed point translation
};

// represents an animation track for a single node
struct AnimTrack {
    vector<Keyframe> frames;
};

// represents a full animation
struct Animation {
    string name;
    int duration; // total frames in this animation
    vector<AnimTrack> nodeTracks; // one track per node in the model
};

// represents a body part (e.g., Torso, Arm)
struct AnimNode {
    int id;
    int parentId; // -1 if root
    u32* displayList; 
    u32 displayListSize; 
    vector<int> children; // auto-populated by the controller
    v16 pivotX, pivotY, pivotZ;
};

class AnimationController {
    public:
        AnimationController();
        
        // loads the auto-generated data
        void loadModel(const vector<AnimNode>& nodes, const vector<Animation>& anims);
        
        // --- Playback Controls ---
        void set(int animIndex, bool loop = true); // Sets the active animation, frame to 0, and looping state
        void play();             // Starts playing the current animation
        void stop();             // Instantly stops the animation and skips to frame 0
        void pause();            // Lets the animation finish its current cycle gracefully, then stops
        
        // advance the animation by 1 frame (call once per VBlank)
        void update();
        
        // submits the matrices and geometry to the GX Engine
        void render();

        bool isAnimationPlaying() const { return isPlaying; }
        int getCurrentAnimIndex() const { return currentAnimIndex; }

    private:
        void renderNode(int nodeId);
        Keyframe getInterpolatedFrame(const AnimTrack& track, int currentTime);

        vector<AnimNode> modelNodes;
        vector<Animation> animations;
        
        int currentAnimIndex = -1;
        int currentFrame = 0;
        bool isPlaying = false;
        bool isFinishing = false; // Flags if the animation is winding down
        bool isLooping = true;    // Flags if the animation repeats automatically
        
        vector<int> rootNodes; 
};