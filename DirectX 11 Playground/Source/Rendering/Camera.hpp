#pragma once

#include <DirectXMath.h>
using namespace DirectX;
#include <array>

class Camera
{
public:
	Camera() = default;
	Camera(const Camera&) = delete;

	void Setup(float AspectRatio);
	void Update();

	void SetPosition(const DirectX::XMVECTOR NewPosition);
	void SetPosition(float NewPosition[4]);

	void ResetPitch();
	void ResetYaw();
	void ResetCamera();

	XMMATRIX GetView() const;
	XMMATRIX GetProjection() const;
	XMMATRIX GetViewProjection();

	XMVECTOR GetPosition() const;
	XMVECTOR GetTarget() const;
	XMVECTOR GetUp() const;

	void DrawGUI();
	void OnAspectRatioChange(float AspectRatio);

private:
	XMMATRIX m_View				{ XMMATRIX() };
	XMMATRIX m_Projection		{ XMMATRIX() };
	XMMATRIX m_ViewProjection	{ XMMATRIX() };

	XMVECTOR m_Position			{ XMVectorSet(0.0f, 10.0f, -25.0f, 0.0f) };
	XMVECTOR m_Target			{ XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f) };
	XMVECTOR m_Up				{ XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };

	// For camera resetting
	XMVECTOR m_Default_Position	{ XMVectorSet(0.0f, 10.0f, -25.0f, 0.0f) };
	XMVECTOR m_Default_Target	{ XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f) };
	XMVECTOR m_Default_Up		{ XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };

	XMMATRIX m_RotationX		{ XMMATRIX() };
	XMMATRIX m_RotationY		{ XMMATRIX() };

	XMMATRIX m_RotationMatrix	{ XMMATRIX() };

	XMVECTOR m_Forward			{ XMVectorSet(0.0f, 0.0, 1.0f, 0.0f) };
	XMVECTOR m_Right			{ XMVectorSet(1.0f, 0.0, 0.0f, 0.0f) };
	XMVECTOR m_Upward			{ XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };

	XMVECTOR m_Default_Forward	{ XMVectorSet(0.0f, 0.0, 1.0f, 0.0f) };
	XMVECTOR m_Default_Right	{ XMVectorSet(1.0f, 0.0, 0.0f, 0.0f) };
	XMVECTOR m_Default_Upward	{ XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };

public:
	float MoveBackForward	{ 0.0f };
	float MoveLeftRight		{ 0.0f };
	float MoveUpDown		{ 0.0f } ;

	float Pitch	{ 0.0f };
	float Yaw	{ 0.0f };

	float CameraSpeed{ 25.0f };
	std::array<float, 3> CameraSlider{ XMVectorGetX(m_Position), XMVectorGetY(m_Position), XMVectorGetZ(m_Position) };

	float m_zNear	{ 0.1f };
	float m_zFar	{ 50000.0f };

};

