#pragma once
#include <nds.h>
#include <vector>

using namespace std;

struct Keyframe {
    int time;             
    s16 rotX, rotY, rotZ; 
    s16 posX, posY, posZ; 
};

struct AnimTrack {
    vector<Keyframe> frames;
};

struct Animation {
    int duration; 
    vector<AnimTrack> nodeTracks; 
};

struct AnimNode {
    int id;
    int parentId; 
    vector<u32> displayList;
    u32 displayListSize; 
    vector<int> children; 
    
    // Absolute pivot loaded from the file
    v16 pivotX, pivotY, pivotZ;
};

class AnimationController {
    public:
        AnimationController();
        
        bool loadModel(const char* filepath);
        
        void set(int animIndex, bool loop = true); 
        void play();             
        void stop();             
        void pause();            
        
        void update();
        void render();

        bool isAnimationPlaying() const { return isPlaying; }
        int getCurrentAnimIndex() const { return currentAnimIndex; }

    private:
        void renderNode(int nodeId);
        Keyframe getInterpolatedFrame(const AnimTrack& track, int currentTime, int nodeId);

        vector<AnimNode> modelNodes;
        vector<Animation> animations;
        
        vector<int> trackIndices; 
        
        int currentAnimIndex = -1;
        int currentFrame = 0;
        bool isPlaying = false;
        bool isFinishing = false; 
        bool isLooping = true;    
        
        vector<int> rootNodes; 
};