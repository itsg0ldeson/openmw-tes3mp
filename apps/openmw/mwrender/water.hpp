#ifndef GAME_MWRENDER_WATER_H
#define GAME_MWRENDER_WATER_H

#include <OgrePlane.h>
#include <OgreRenderQueue.h>
#include <OgreRenderQueueListener.h>
#include <OgreRenderTargetListener.h>
#include <OgreMaterial.h>
#include <OgreTexture.h>

#include <components/esm/loadcell.hpp>
#include <components/settings/settings.hpp>

#include "renderconst.hpp"

#include <extern/shiny/Main/MaterialInstance.hpp>

namespace Ogre
{
    class Camera;
    class SceneManager;
    class SceneNode;
    class Entity;
    class Vector3;
    struct RenderTargetEvent;
}

namespace MWRender {

    class SkyManager;
    class RenderingManager;
    class RippleSimulation;
    class Refraction;

    class Reflection
    {
    public:
        Reflection(Ogre::SceneManager* sceneManager)
            :   mSceneMgr(sceneManager) {}
        virtual ~Reflection() {}

        virtual void setWaterPlane (Ogre::Plane plane) {}
        virtual void setParentCamera (Ogre::Camera* parent) { mParentCamera = parent; }
        void setUnderwater(bool underwater) { mIsUnderwater = underwater; }
        virtual void setActive (bool active) {}
        virtual void setViewportBackground(Ogre::ColourValue colour) {}
        virtual void update() {}
        virtual void setVisibilityMask (int flags) {}

    protected:
        Ogre::Camera* mCamera;
        Ogre::Camera* mParentCamera;
        Ogre::TexturePtr mTexture;
        Ogre::SceneManager* mSceneMgr;
        bool mIsUnderwater;
    };

    class CubeReflection : public Reflection
    {
    public:
        CubeReflection(Ogre::SceneManager* sceneManager);
        virtual ~CubeReflection();

        virtual void update();
    protected:
        Ogre::RenderTarget* mRenderTargets[6];
    };

    class PlaneReflection : public Reflection, public Ogre::RenderQueueListener, public Ogre::RenderTargetListener
    {
    public:
        PlaneReflection(Ogre::SceneManager* sceneManager, SkyManager* sky);
        virtual ~PlaneReflection();

        virtual void setWaterPlane (Ogre::Plane plane);
        virtual void setActive (bool active);
        virtual void setVisibilityMask (int flags);

        void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
        void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);

        void renderQueueStarted (Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation);
        void renderQueueEnded (Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation);

        virtual void setViewportBackground(Ogre::ColourValue colour);

    protected:
        Ogre::RenderTarget* mRenderTarget;
        SkyManager* mSky;
        Ogre::Plane mWaterPlane;
        Ogre::Plane mErrorPlane;
        bool mRenderActive;
    };

    /// Water rendering
    class Water : public Ogre::RenderTargetListener, public Ogre::RenderQueueListener, public sh::MaterialInstanceListener
    {
        static const int CELL_SIZE = 8192;
        Ogre::Camera *mCamera;
        Ogre::SceneManager *mSceneMgr;

        Ogre::Plane mWaterPlane;

        Ogre::SceneNode *mWaterNode;
        Ogre::Entity *mWater;

        //Ogre::SceneNode* mUnderwaterDome;

        bool mIsUnderwater;
        bool mActive;
        bool mToggled;
        int mTop;

        float mWaterTimer;


        Ogre::Vector3 getSceneNodeCoordinates(int gridX, int gridY);

    protected:
        void applyRTT();
        void applyVisibilityMask();

        void updateVisible();

        RenderingManager* mRendering;
        SkyManager* mSky;

        std::string mCompositorName;

        Ogre::MaterialPtr mMaterial;

        bool mUnderwaterEffect;
        int mVisibilityFlags;

        Reflection* mReflection;
        Refraction* mRefraction;
        RippleSimulation* mSimulation;

    public:
        Water (Ogre::Camera *camera, RenderingManager* rend, const ESM::Cell* cell);
        ~Water();

        void setActive(bool active);

        void toggle();
        void update(float dt, Ogre::Vector3 player);

        void assignTextures();

        void setViewportBackground(const Ogre::ColourValue& bg);

        void processChangedSettings(const Settings::CategorySettingVector& settings);

        /// Updates underwater state accordingly
        void updateUnderwater(bool underwater);
        void changeCell(const ESM::Cell* cell);
        void setHeight(const float height);

        virtual void requestedConfiguration (sh::MaterialInstance* m, const std::string& configuration);
        virtual void createdConfiguration (sh::MaterialInstance* m, const std::string& configuration);

    };

}

#endif
