#include "Camera.hpp"
#include "ImGui/imgui.h"

using namespace DirectX;

void Camera::Setup(float AspectRatio)
{
	m_Position	= m_Default_Position;
	m_Target	= m_Default_Target;
	m_Up		= m_Default_Up;

	m_View = XMMatrixLookAtLH(m_Position, m_Target, m_Up);
	const float FoV{ 0.4f * XM_PI };
	m_Projection = XMMatrixPerspectiveFovLH(FoV, AspectRatio, m_zNear, m_zFar);
}

void Camera::Update()
{
	m_RotationMatrix = XMMatrixRotationRollPitchYaw(Pitch, Yaw, 0.0f);
	m_Target = XMVector3Normalize(XMVector3TransformCoord(m_Default_Forward, m_RotationMatrix));
	//m_Target = XMVector3Normalize(m_Target);

	XMMATRIX rotation{ XMMatrixRotationY(Yaw) };

	m_Forward = XMVector3TransformCoord(m_Default_Forward, rotation);
	m_Right = XMVector3TransformCoord(m_Default_Right, rotation);
	m_Up = XMVector3TransformCoord(m_Up, rotation);

	m_Position += (MoveBackForward * m_Forward);
	m_Position += (MoveLeftRight * m_Right);
	m_Position += (MoveUpDown * m_Upward);

	MoveBackForward = 0.0f;
	MoveLeftRight = 0.0f;
	MoveUpDown = 0.0f;

	m_Target = (m_Position + m_Target);

	m_View = XMMatrixLookAtLH(m_Position, m_Target, m_Up);
	CameraSlider = { XMVectorGetX(m_Position), XMVectorGetY(m_Position), XMVectorGetZ(m_Position) };
}

void Camera::SetPosition(const DirectX::XMVECTOR NewPosition)
{
	m_Position = NewPosition;
}

void Camera::SetPosition(float NewPosition[4])
{
	m_Position = XMVectorSet(NewPosition[0], NewPosition[1], NewPosition[2], 0.0f);
}

void Camera::ResetPitch()
{
	Pitch = 0.0f;
}

void Camera::ResetYaw()
{
	Yaw = 0.0f;
}

void Camera::ResetCamera()
{
	m_Position	= m_Default_Position;
	m_Target	= m_Default_Target;
	m_Up		= m_Default_Up;
	Pitch		= 0.0f;
	Yaw			= 0.0f;
}

DirectX::XMMATRIX Camera::GetView() const
{
	return m_View;
}

DirectX::XMMATRIX Camera::GetProjection() const
{
	return m_Projection;
}

DirectX::XMMATRIX Camera::GetViewProjection()
{
	return DirectX::XMMatrixMultiply(m_View, m_Projection);
}

DirectX::XMVECTOR Camera::GetPosition() const
{
	return m_Position;
}

DirectX::XMVECTOR Camera::GetTarget() const
{
	return m_Target;
}

DirectX::XMVECTOR Camera::GetUp() const
{
	return m_Up;
}

void Camera::DrawGUI()
{
	//ImGui::Begin("Camera");
	if (ImGui::DragFloat3("XYZ Coords", CameraSlider.data(), CameraSpeed))
	{
		SetPosition(CameraSlider.data());
		Update();
	}
	ImGui::SliderFloat("Speed", &CameraSpeed, 1.0f, 1000.0f);
	if (ImGui::Button("Reset"))
	{
		ResetCamera();
		Update();
	}

	if (ImGui::DragFloat("Rotation X", &Pitch, 0.25f, -360.0f, 360.0f))
		Update();
	if (ImGui::DragFloat("Rotation Y", &Yaw, 0.25f, -360.0f, 360.0f))
		Update();
	//ImGui::DragFloat("Rotation Z", 0.0f, 0.25f, -360.0f, 360.0f);


	//ImGui::End();
}

void Camera::OnAspectRatioChange(float AspectRatio)
{
	const float FoV{ 0.4f * XM_PI };
	m_Projection = XMMatrixPerspectiveFovLH(FoV, AspectRatio, m_zNear, m_zFar);
}
