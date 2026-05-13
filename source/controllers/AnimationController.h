#pragma once
#include <nds.h>
#include <vector>

using namespace std;

struct Keyframe
{
    int time;             // frame number
    s16 rotX, rotY, rotZ; // rotation  (32768 == 360°)
    s16 posX, posY, posZ; // translation in v16 fixed-point
};

struct AnimTrack
{
    vector<Keyframe> frames;
};

struct Animation
{
    int duration; // total frame count
    vector<AnimTrack> nodeTracks;
};

// A SubList holds the display-list words for one texture group within a node.
// texSlot == -1 means "no texture" (bind nothing before calling this list).
struct SubList
{
    int texSlot;
    vector<u32> displayList; // displayList[0] == dlSize, rest == words
    u32 displayListSize;
};

struct AnimNode
{
    int id;
    int parentId;
    vector<SubList> subLists; // one entry per material/texture group
    vector<int> children;

    // Absolute pivot in v16 fixed-point, loaded from the MDL2 file
    v16 pivotX, pivotY, pivotZ;
};
class AnimationController
{
public:
    AnimationController();
    ~AnimationController();

    // Load the MDL2 (or legacy MDL1) binary produced by obj2model.py.
    bool loadModel(const char *filepath);

    // Upload textures to VRAM after loadModel().
    // Call the generated <modelname>_loadTextures() helper from your .h file
    // rather than calling this directly — it supplies the correct sizes/formats.
    //
    //   count   : number of texture slots (== MODEL_XXX_TEX_COUNT)
    //   bitmaps : array of pointers to grit-generated bitmap data (one per slot)
    //   widths  : NDS-valid pixel widths  (power-of-two, 8–1024)
    //   heights : NDS-valid pixel heights (power-of-two, 8–1024)
    //   isRGBA  : true → GL_RGBA (16-bit), false → GL_RGB
    bool loadTextures(int count,
                      const unsigned int **bitmaps,
                      const int *widths,
                      const int *heights,
                      const bool *isRGBA);

    // Animation control
    void set(int animIndex, bool loop = true);
    void play();
    void stop();
    void pause();

    // Call once per frame (advances the frame counter)
    void update();

    // Call once per frame (renders all nodes into the NDS geometry engine)
    void render();

    bool isAnimationPlaying() const { return isPlaying; }
    int getCurrentAnimIndex() const { return currentAnimIndex; }

private:
    void renderNode(int nodeId);
    Keyframe getInterpolatedFrame(const AnimTrack &track, int currentTime, int nodeId);
    static int textureSizeToEnum(int px);

    vector<AnimNode> modelNodes;
    vector<Animation> animations;
    vector<int> trackIndices; // per-node cached search position
    vector<int> rootNodes;

    // One GL texture ID per texture slot.  Empty if loadTextures() not called.
    vector<int> textureIDs;

    int currentAnimIndex = -1;
    int currentFrame = 0;
    bool isPlaying = false;
    bool isFinishing = false;
    bool isLooping = true;
};
