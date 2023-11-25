/*
 * Copyright (c) 2023, chunquedong
 *
 * This file is part of MGP project
 * Licensed under the GNU LESSER GENERAL PUBLIC LICENSE
 *
 */
#ifndef CAMERACTRL_H_
#define CAMERACTRL_H_

#include "scene/Scene.h"
#include "InputListener.h"

namespace mgp {

class CameraCtrl : public InputListener {
public:
    virtual ~CameraCtrl() {}
    
    virtual void update(float elapsedTime) {}

    virtual void touchEvent(MotionEvent& evt) {}

    virtual void keyEvent(Keyboard evt) {}

    virtual bool mouseEvent(Mouse evt) { return false; }
};

class SceneView;
class EditorCameraCtrl : public CameraCtrl {
    int _prevX;
    int _prevY;
    Camera* _camera;

    float _pitch;
    float _yaw;
    double _surfaceDistance;
    Vector3 _rotateCenter;
    bool _dirty = false;
public:

    SceneView* sceneView;
    EditorCameraCtrl();

    void setRotateCenter(const Vector3& c);

    void setRotate(float pitch, float yaw);
    void getRotate(float* pitch, float* yaw) { *pitch = _pitch; *yaw = _yaw; }

    void setCamera(Camera* camera) { _camera = camera; }

    void update(float elapsedTime);

    void touchEvent(MotionEvent& evt);

    void keyEvent(Keyboard evt);

    bool mouseEvent(Mouse evt);

private:
    bool updateSurfaceDistance();
};

}

#endif