#include "base/Base.h"
#include "Camera.h"
#include "platform/Toolkit.h"
#include "scene/Node.h"
#include "platform/Toolkit.h"
//#include "physics/PhysicsController.h"

#define CAMERA_FIELD_OF_VIEW 60.0f
#define CAMERA_SIZE 5.0f
#define CAMERA_CLIP_PLANE_NEAR 0.1f
#define CAMERA_CLIP_PLANE_FAR 1000.f

// Camera dirty bits
#define CAMERA_DIRTY_VIEW 1
#define CAMERA_DIRTY_PROJ 2
#define CAMERA_DIRTY_VIEW_PROJ 4
#define CAMERA_DIRTY_INV_VIEW 8
#define CAMERA_DIRTY_INV_VIEW_PROJ 16
#define CAMERA_DIRTY_BOUNDS 32
#define CAMERA_DIRTY_ALL (CAMERA_DIRTY_VIEW | CAMERA_DIRTY_PROJ | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_INV_VIEW | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_BOUNDS)

// Other misc camera bits
#define CAMERA_CUSTOM_PROJECTION 64

namespace mgp
{

Camera::Camera() : _type(PERSPECTIVE), _fieldOfView(CAMERA_FIELD_OF_VIEW), _aspectRatio(1), _nearPlane(CAMERA_CLIP_PLANE_NEAR), _farPlane(CAMERA_CLIP_PLANE_FAR),
    _bits(CAMERA_DIRTY_ALL), _listeners(NULL) {

}

Camera::Camera(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
    : _type(PERSPECTIVE), _fieldOfView(fieldOfView), _aspectRatio(aspectRatio), _nearPlane(nearPlane), _farPlane(farPlane),
    _bits(CAMERA_DIRTY_ALL), _listeners(NULL)
{
}

Camera::Camera(float zoomX, float zoomY, float aspectRatio, float nearPlane, float farPlane)
    : _type(ORTHOGRAPHIC), _aspectRatio(aspectRatio), _nearPlane(nearPlane), _farPlane(farPlane),
	_bits(CAMERA_DIRTY_ALL), _listeners(NULL)
{
    // Orthographic camera.
    _zoom[0] = zoomX;
    _zoom[1] = zoomY;
}

void Camera::copyFrom(const Camera& other) {
    other.getViewMatrix();
    _type = other._type;
    _fieldOfView = other._fieldOfView;
    _zoom[0] = other._zoom[0];
    _zoom[1] = other._zoom[1];
    _aspectRatio = other._aspectRatio;
    _nearPlane = other._nearPlane;
    _farPlane = other._farPlane;
    _view = other._view;
    _projection = other._projection;
    _viewProjection = other._viewProjection;
    _inverseView = other._inverseView;
    _inverseViewProjection = other._inverseViewProjection;
    _bounds = other._bounds;
    _bits = other._bits;
    //_node = other._node;
}

Camera::~Camera()
{
    SAFE_DELETE(_listeners);
}

UPtr<Camera> Camera::createPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
    return UPtr<Camera>(new Camera(fieldOfView, aspectRatio, nearPlane, farPlane));
}

UPtr<Camera> Camera::createOrthographic(float zoomX, float zoomY, float aspectRatio, float nearPlane, float farPlane)
{
    return UPtr<Camera>(new Camera(zoomX, zoomY, aspectRatio, nearPlane, farPlane));
}

Camera::Type Camera::getCameraType() const
{
    return _type;
}

float Camera::getFieldOfView() const
{
    GP_ASSERT(_type == Camera::PERSPECTIVE);

    return _fieldOfView;
}

void Camera::setFieldOfView(float fieldOfView)
{
    GP_ASSERT(_type == Camera::PERSPECTIVE);

    _fieldOfView = fieldOfView;
    _bits |= CAMERA_DIRTY_PROJ | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_BOUNDS;
    cameraChanged();
}

float Camera::getZoomX() const
{
    GP_ASSERT(_type == Camera::ORTHOGRAPHIC);

    return _zoom[0];
}

void Camera::setZoomX(float zoomX)
{
    GP_ASSERT(_type == Camera::ORTHOGRAPHIC);

    _zoom[0] = zoomX;
    _bits |= CAMERA_DIRTY_PROJ | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_BOUNDS;
    cameraChanged();
}

float Camera::getZoomY() const
{
    GP_ASSERT(_type == Camera::ORTHOGRAPHIC);

    return _zoom[1];
}

void Camera::setZoomY(float zoomY)
{
    GP_ASSERT(_type == Camera::ORTHOGRAPHIC);

    _zoom[1] = zoomY;
    _bits |= CAMERA_DIRTY_PROJ | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_BOUNDS;
    cameraChanged();
}

float Camera::getAspectRatio() const
{
    return _aspectRatio;
}

void Camera::setAspectRatio(float aspectRatio)
{
    _aspectRatio = aspectRatio;
    _bits |= CAMERA_DIRTY_PROJ | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_BOUNDS;
    cameraChanged();
}

float Camera::getNearPlane() const
{
    return _nearPlane;
}

void Camera::setNearPlane(float nearPlane)
{
    _nearPlane = nearPlane;
    _bits |= CAMERA_DIRTY_PROJ | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_BOUNDS;
    cameraChanged();
}

float Camera::getFarPlane() const
{
    return _farPlane;
}

void Camera::setFarPlane(float farPlane)
{
    _farPlane = farPlane;
    _bits |= CAMERA_DIRTY_PROJ | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_BOUNDS;
    cameraChanged();
}

Node* Camera::getNode() const
{
    return _node;
}

void Camera::setNode(Node* node)
{
    if (_node != node)
    {
        if (_node)
        {
            _node->removeListener(this);
        }

        // Connect the new node.
        _node = node;

        if (_node)
        {
            _node->addListener(this);
        }

        _bits |= CAMERA_DIRTY_VIEW | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_INV_VIEW | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_BOUNDS;
        cameraChanged();
    }
}

const Matrix& Camera::getViewMatrix() const
{
    if (_bits & CAMERA_DIRTY_VIEW)
    {
        if (_node)
        {
            // The view matrix is the inverse of our transform matrix.
            _node->getWorldMatrix().invert(&_view);
        }
        else
        {
            _view.setIdentity();
        }

        _bits &= ~CAMERA_DIRTY_VIEW;
    }

    return _view;
}

const Matrix& Camera::getInverseViewMatrix() const
{
    if (_bits & CAMERA_DIRTY_INV_VIEW)
    {
        getViewMatrix().invert(&_inverseView);

        _bits &= ~CAMERA_DIRTY_INV_VIEW;
    }

    return _inverseView;
}

const Matrix& Camera::getProjectionMatrix() const
{
    if (!(_bits & CAMERA_CUSTOM_PROJECTION) && (_bits & CAMERA_DIRTY_PROJ))
    {
        if (_type == PERSPECTIVE)
        {
            Matrix::createPerspective(_fieldOfView, _aspectRatio, _nearPlane, _farPlane, &_projection);
        }
        else
        {
            // Create an ortho projection with the origin at the bottom left of the viewport, +X to the right and +Y up.
            Matrix::createOrthographic(_zoom[0], _zoom[1], _nearPlane, _farPlane, &_projection);
        }

        _bits &= ~CAMERA_DIRTY_PROJ;
    }

    return _projection;
}

void Camera::setProjectionMatrix(const Matrix& matrix)
{
    _projection = matrix;
    _bits |= CAMERA_CUSTOM_PROJECTION;
    _bits |= CAMERA_DIRTY_PROJ | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_BOUNDS;

    Matrix projInverse = _projection;
    projInverse.invert();

    Vector4 nearPoint = projInverse * Vector4(0.0f, 0.0f, -1.0f, 1.0f);
    Vector4 farPoint = projInverse * Vector4(0.0f, 0.0f, 1.0f, 1.0f);

    _nearPlane = -nearPoint.z / nearPoint.w;
    _farPlane = -farPoint.z / farPoint.w;

    cameraChanged();
}

void Camera::resetProjectionMatrix()
{
    if (_bits & CAMERA_CUSTOM_PROJECTION)
    {
        _bits &= ~CAMERA_CUSTOM_PROJECTION;
        _bits |= CAMERA_DIRTY_PROJ | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_BOUNDS;

        cameraChanged();
    }
}

const Matrix& Camera::getViewProjectionMatrix() const
{
    if (_bits & CAMERA_DIRTY_VIEW_PROJ)
    {
        Matrix::multiply(getProjectionMatrix(), getViewMatrix(), &_viewProjection);

        _bits &= ~CAMERA_DIRTY_VIEW_PROJ;
    }

    return _viewProjection;
}

const Matrix& Camera::getInverseViewProjectionMatrix() const
{
    if (_bits & CAMERA_DIRTY_INV_VIEW_PROJ)
    {
        getViewProjectionMatrix().invert(&_inverseViewProjection);

        _bits &= ~CAMERA_DIRTY_INV_VIEW_PROJ;
    }

    return _inverseViewProjection;
}

const Frustum& Camera::getFrustum() const
{
    if (_bits & CAMERA_DIRTY_BOUNDS)
    {
        // Update our bounding frustum from our view projection matrix.
        _bounds.set(getViewProjectionMatrix());

        _bits &= ~CAMERA_DIRTY_BOUNDS;
    }

    return _bounds;
}

void Camera::project(const Rectangle& viewport, const Vector3& position, float* x, float* y, float* depth) const
{
    GP_ASSERT(x);
    GP_ASSERT(y);

    // Transform the point to clip-space.
    Vector4 clipPos;
    getViewProjectionMatrix().transformVector(Vector4(position.x, position.y, position.z, 1.0f), &clipPos);

    // Compute normalized device coordinates.
    if (clipPos.w == 0.0f) {
        *x = 0;
        *y = 0;
        *depth = 0;
        return;
    }
    //GP_ASSERT(clipPos.w != 0.0f);
    
    float ndcX = clipPos.x / clipPos.w;
    float ndcY = clipPos.y / clipPos.w;

    // Compute screen coordinates by applying our viewport transformation.
    *x = viewport.x + (ndcX + 1.0f) * 0.5f * viewport.width;
    *y = viewport.y + (1.0f - (ndcY + 1.0f) * 0.5f) * viewport.height;
    if (depth)
    {
        float ndcZ = clipPos.z / clipPos.w;
        *depth = (ndcZ + 1.0f) / 2.0f;
    }
}

void Camera::project(const Rectangle& viewport, const Vector3& position, Vector2* out) const
{
    GP_ASSERT(out);
    float x, y;
    project(viewport, position, &x, &y);
    out->set(x, y);
}

void Camera::project(const Rectangle& viewport, const Vector3& position, Vector3* out) const
{
    GP_ASSERT(out);
    float x, y, depth;
    project(viewport, position, &x, &y, &depth);
    out->set(x, y, depth);
}

void Camera::unproject(const Rectangle& viewport, float x, float y, float depth, Vector3* dst) const
{
    GP_ASSERT(dst);
    
    // Create our screen space position in NDC.
    GP_ASSERT(viewport.width != 0.0f && viewport.height != 0.0f);
    Vector4 screen((x - viewport.x) / viewport.width, ((viewport.height - y) - viewport.y) / viewport.height, depth, 1.0f);

    // Map to range -1 to 1.
    screen.x = screen.x * 2.0f - 1.0f;
    screen.y = screen.y * 2.0f - 1.0f;
    screen.z = screen.z * 2.0f - 1.0f;

    // Transform the screen-space NDC by our inverse view projection matrix.
    getInverseViewProjectionMatrix().transformVector(screen, &screen);

    // Divide by our W coordinate.
    if (screen.w != 0.0f)
    {
        screen.x /= screen.w;
        screen.y /= screen.w;
        screen.z /= screen.w;
    }

    dst->set(screen.x, screen.y, screen.z);
}

void Camera::pickRay(const Rectangle& viewport, float x, float y, Ray* dst) const
{
    GP_ASSERT(dst);

    // Get the world-space position at the near clip plane.
    Vector3 nearPoint;
    unproject(viewport, x, y, 0.0f, &nearPoint);

    // Get the world-space position at the far clip plane.
    Vector3 farPoint;
    unproject(viewport, x, y, 1.0f, &farPoint);

    // Set the direction of the ray.
    Vector3 direction;
    Vector3::subtract(farPoint, nearPoint, &direction);
    direction.normalize();

    dst->set(nearPoint, direction);
}

UPtr<Camera> Camera::clone(NodeCloneContext& context)
{
    UPtr<Camera> cameraClone;
    if (getCameraType() == PERSPECTIVE)
    {
        cameraClone = createPerspective(_fieldOfView, _aspectRatio, _nearPlane, _farPlane);
    }
    else if (getCameraType() == ORTHOGRAPHIC)
    {
        cameraClone = createOrthographic(getZoomX(), getZoomY(), getAspectRatio(), _nearPlane, _farPlane);
    }
    GP_ASSERT(cameraClone.get());

    if (Node* node = context.findClonedNode(getNode()))
    {
        cameraClone->setNode(node);
    }
    return cameraClone;
}

void Camera::transformChanged(Transform* transform, long cookie)
{
    _bits |= CAMERA_DIRTY_VIEW | CAMERA_DIRTY_INV_VIEW | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_BOUNDS;

    cameraChanged();
}

void Camera::cameraChanged()
{
    if (_listeners == NULL)
        return;

    for (std::list<Camera::Listener*>::iterator itr = _listeners->begin(); itr != _listeners->end(); ++itr)
    {
        Camera::Listener* listener = (*itr);
        listener->cameraChanged(this);
    }
}

void Camera::addListener(Camera::Listener* listener)
{
    GP_ASSERT(listener);

    if (_listeners == NULL)
        _listeners = new std::list<Camera::Listener*>();

    _listeners->push_back(listener);
}

void Camera::removeListener(Camera::Listener* listener)
{
    GP_ASSERT(listener);

    if (_listeners)
    {
        for (std::list<Camera::Listener*>::iterator itr = _listeners->begin(); itr != _listeners->end(); ++itr)
        {
            if ((*itr) == listener)
            {
                _listeners->erase(itr);
                break;
            }
        }
    }
}


Serializable* Camera::createObject()
{
    return new Camera();
}

std::string Camera::enumToString(const std::string& enumName, int value)
{
    if (enumName.compare("mgp::Camera::Mode") == 0)
    {
        switch (value)
        {
            case static_cast<int>(Type::PERSPECTIVE):
                return "Perspective";
            case static_cast<int>(Type::ORTHOGRAPHIC):
                return "Orthograhic";
            default:
                break;
        }
    }
    return "";
}

int Camera::enumParse(const std::string& enumName, const std::string& str)
{
    if (enumName.compare("mgp::Camera::Mode") == 0)
    {
        if (str.compare("Perspective") == 0)
            return static_cast<int>(Type::PERSPECTIVE);
        else if (str.compare("Orthograhic") == 0)
            return static_cast<int>(Type::ORTHOGRAPHIC);
    }
    return static_cast<int>(Type::PERSPECTIVE);
}

std::string Camera::getClassName()
{
    return "mgp::Camera";
}

void Camera::onSerialize(Serializer* serializer)
{
    serializer->writeEnum("mode", "mgp::Camera::Mode", static_cast<int>(_type), -1);
    if (_type == Type::PERSPECTIVE)
    {
        serializer->writeFloat("fieldOfView", _fieldOfView, CAMERA_FIELD_OF_VIEW);
    }
    else
    {
        serializer->writeFloat("size", _zoom[0], CAMERA_SIZE);
    }
    serializer->writeFloat("clipPlaneNear", _nearPlane, CAMERA_CLIP_PLANE_NEAR);
    serializer->writeFloat("clipPlaneFar", _farPlane, CAMERA_CLIP_PLANE_FAR);
}

void Camera::onDeserialize(Serializer* serializer)
{
    _type = static_cast<Camera::Type>(serializer->readEnum("mode", "mgp::Camera::Mode", -1));
    if (_type == Type::PERSPECTIVE)
    {
        _fieldOfView = serializer->readFloat("fieldOfView", CAMERA_FIELD_OF_VIEW);
    }
    else
    {
        _zoom[0] = serializer->readFloat("size", CAMERA_SIZE);
        _zoom[1] = _zoom[0];
    }
    _nearPlane = serializer->readFloat("clipPlaneNear", CAMERA_CLIP_PLANE_NEAR);
    _farPlane = serializer->readFloat("clipPlaneFar", CAMERA_CLIP_PLANE_FAR);
}


}
