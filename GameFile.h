#pragma once
#include "StateMachine.h"
#include "Math.h"

class GameFile {
public:
	struct Base {
		ComPtr<ID3D11Buffer> vertexBuffer;
		ComPtr<ID3D11Buffer> indexBuffer;
		ComPtr<ID3D11Buffer> constantBuffer;
		Math::float2 currentPos;
		Math::float2 prevPos;
		void InitBuffers() {
			Graphics::Vertex data[] =
			{
				XMFLOAT2(-0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0, 1),
				XMFLOAT2(-0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0, 0),
				XMFLOAT2(0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1, 0),
				XMFLOAT2(0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1, 1)
			};
			Graphics::CreateVertexBuffer(vertexBuffer, data, ARRAYSIZE(data));
			Graphics::CreateIndexBuffer(indexBuffer);
			Graphics::CreateConstantBuffer<Graphics::Constants>(constantBuffer);
		}
		virtual void Update() = 0;
		void Render() {
			Graphics::SetConstantValues<Graphics::Constants>(this->constantBuffer.Get(), { XMFLOAT2{currentPos.x / Structures::Window::GetWidth(),
				currentPos.y / Structures::Window::GetHeight()}, XMFLOAT2{0, 0}, XMFLOAT4{1.0f, 0, 0, 0} });
			Graphics::d3dDeviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
			Graphics::d3dDeviceContext->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
			Graphics::d3dDeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			Graphics::d3dDeviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &Graphics::stride,
				&Graphics::offset);
			Graphics::d3dDeviceContext->DrawIndexed(6, 0, 0);
		}
		void StayInBound() {
			float n_xScreenMax = Graphics::GetEyeDistance().z * (float)Structures::Window::GetWidth();
			float n_yScreenMax = Graphics::GetEyeDistance().z * (float)Structures::Window::GetHeight();
			if (currentPos.x > n_xScreenMax) currentPos.x = -n_xScreenMax;
			if (currentPos.x < -n_xScreenMax) currentPos.x = n_xScreenMax;
			if (currentPos.y > n_yScreenMax) currentPos.y = -n_yScreenMax;
			if (currentPos.y < -n_yScreenMax) currentPos.y = n_yScreenMax;
		}
	};
	struct Node : Base {
		Node() {
			currentPos = Math::float2();
			prevPos = Math::float2();
			InitBuffers();
		}
		Node(Math::float2 pos) {
			currentPos = pos;
			prevPos = Math::float2();
			InitBuffers();
		}
		void Update() override {

		}
	};
	struct Head : Base {
		Head() {
			currentPos = Math::float2();
			prevPos = Math::float2();
			InitBuffers();
		}
		void Update() override {
			prevPos = currentPos;
			if(BaseStateMachine::lastKeyHit == 'W') {currentPos += Math::float2(0, 1) * Structures::Window::GetWidth(); }
			else if(BaseStateMachine::lastKeyHit == 'S') {currentPos += Math::float2(0, -1) * Structures::Window::GetWidth(); }
			else if(BaseStateMachine::lastKeyHit == 'A') {currentPos += Math::float2(-1, 0) * Structures::Window::GetWidth(); }
			else if (BaseStateMachine::lastKeyHit == 'D') { currentPos += Math::float2(1, 0) * Structures::Window::GetWidth(); }
			else { currentPos += Math::float2(0, 1) * Structures::Window::GetWidth(); }
		}
	}; 
	struct Apple {
		ComPtr<ID3D11Buffer> vertexBuffer;
		ComPtr<ID3D11Buffer> indexBuffer;
		ComPtr<ID3D11Buffer> constantBuffer;
		Math::float2 position;
		Apple() {
			srand(time(0));
			float maxLimit = Graphics::GetEyeDistance().z * 2 + 1;
			int xScreenSize = Structures::Window::GetWidth();
			int yScreenSize = Structures::Window::GetHeight();
			Math::float2 offset = Math::float2(Graphics::GetEyeDistance().z * xScreenSize, Graphics::GetEyeDistance().z * yScreenSize);
			position = Math::float2(Math::randInt(0, (uint32_t)maxLimit) * xScreenSize, Math::randInt(0, (uint32_t)maxLimit) * yScreenSize);
			position -= offset;
			Graphics::Vertex data[] =
			{
				XMFLOAT2(-0.5f, -0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0, 1),
				XMFLOAT2(-0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0, 0),
				XMFLOAT2(0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1, 0),
				XMFLOAT2(0.5f, -0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1, 1)
			};
			Graphics::CreateVertexBuffer(vertexBuffer, data, ARRAYSIZE(data));
			Graphics::CreateIndexBuffer(indexBuffer);
			Graphics::CreateConstantBuffer<Graphics::Constants>(constantBuffer);
		}
		void Render() {
			Graphics::SetConstantValues<Graphics::Constants>(this->constantBuffer.Get(), { XMFLOAT2{position.x / Structures::Window::GetWidth(),
				position.y / Structures::Window::GetHeight()}, XMFLOAT2{0, 0}, XMFLOAT4{1.0f, 0, 0, 0} });
			Graphics::d3dDeviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
			Graphics::d3dDeviceContext->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
			Graphics::d3dDeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			Graphics::d3dDeviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &Graphics::stride,
				&Graphics::offset);
			Graphics::d3dDeviceContext->DrawIndexed(6, 0, 0);
		}
		bool Collides(Base* node) {
			return (this->position.x < node->currentPos.x + Structures::Window::GetWidth() &&
				this->position.x + Structures::Window::GetWidth() > node->currentPos.x &&
				this->position.y < node->currentPos.y + Structures::Window::GetHeight() &&
				this->position.y + Structures::Window::GetHeight() > node->currentPos.y);
		}
	};
	typedef Base SnakeBase;
	typedef Node SnakeNode;
	typedef Head SnakeHead;
	typedef Apple apple;
	static int nHighScore;
	static int nScore;
};

class Level {
public:
	enum class ManageLevel {
		CurrentLevel,
		PrevLevel,
		NextLevel
	} nLevel = ManageLevel::CurrentLevel;
	std::string levelName;
	virtual void Render() = 0;
	virtual void FixedUpdate() = 0;
	virtual void Update() = 0;
	virtual void Load() = 0;
	virtual void UnLoad() = 0;
};

class MainLevel : public Level {
public:
	MainLevel() {
		this->nLevel = Level::ManageLevel::CurrentLevel;
	}
	void Update() override {
		if (vApple.back()->Collides(nodes[0])) {
			vApple.pop_back();
			nodes.push_back(new GameFile::SnakeNode(nodes.back()->prevPos));
			GameFile::nScore++;
			vApple.push_back(new GameFile::apple());
		}
		for (int i = 1; i < nodes.size(); i++) {
			if (nodes[i]->currentPos.x == nodes[0]->currentPos.x && nodes[i]->currentPos.y == nodes[0]->currentPos.y) {
				if(GameFile::nScore > GameFile::nHighScore){
					SaveSystem::ClearCurrentFile();
					SaveSystem::WriteData(std::to_string(GameFile::nScore));
				}
				this->nLevel = ManageLevel::NextLevel;
			}
			nodes[i]->prevPos = nodes[i]->currentPos;
			nodes[i]->currentPos = nodes[i - 1]->prevPos;
		}
		for (GameFile::SnakeBase* b : nodes) {
			b->StayInBound();
		}
	}
	void FixedUpdate() override {
		for (GameFile::SnakeBase* n : nodes) n->Update();
	}
	void Render() override {
		using Structures::Camera;
		Graphics::SetConstantValues<Graphics::ProjectionBuffer>(Graphics::projectionBuffer.Get(), {
			Camera::projMatrix, Camera::worldMatrix, Camera::viewMatrix });
		Graphics::Clear(0.5f, 0.5f, 0.5f, 1.0f);
		Graphics::Begin();
		Sleep(75);
		for (GameFile::SnakeBase* b : nodes) {
			b->Render();
		}
		for (GameFile::apple* a : vApple) {
			a->Render();
		}
		Graphics::End();
	}
	void Load() override {
		this->levelName = "MainLevel";
		GameFile::nHighScore = atoi(SaveSystem::ReadLine().c_str());
		nodes.push_back(new GameFile::SnakeHead());
		vApple.push_back(new GameFile::apple());
	}
	void UnLoad() override {
		nodes.clear();
		vApple.clear();
	}
private:
	std::vector<GameFile::SnakeBase*> nodes;
	std::vector<GameFile::apple*> vApple;
};

class MainMenu : public Level {
public:
	MainMenu() {
		this->nLevel = Level::ManageLevel::CurrentLevel;
	}
	void Update() override {
		if (BaseStateMachine::isKeyPressed(VK_RETURN)) 
			this->nLevel = ManageLevel::NextLevel;
	}
	void FixedUpdate() override {

	}
	void Render() override {
		Graphics::GetRenderTarget()->BeginDraw();
		Graphics::GetRenderTarget()->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		Graphics::DrawTextF(L"High Score: " + std::to_wstring(GameFile::nHighScore), 0,
			Structures::Window::GetHeight() / 2, Structures::Window::GetWidth(), 20, Graphics::whiteColor.Get());
		Graphics::DrawTextF(L"Press 'Enter' to play!", 0, Structures::Window::GetHeight() / 2 - 200, 
			Structures::Window::GetWidth(), 20, Graphics::whiteColor.Get());
		Graphics::GetRenderTarget()->EndDraw();
	}
	void Load() override {
		if (strcmp(SaveSystem::GetFileName(), "Score.txt") == 0)
			SaveSystem::FileInit("Score.txt");
		GameFile::nHighScore = atoi(SaveSystem::ReadLine().c_str());
	}
	void UnLoad() override {
		
	}
};

class GameOver : public Level {
public:
	GameOver() {
		this->nLevel = Level::ManageLevel::CurrentLevel;
	}
	void Update() override {
		if (BaseStateMachine::isKeyPressed(VK_RETURN)) 
			this->nLevel = ManageLevel::PrevLevel;
	}
	void FixedUpdate() override {

	}
	void Render() override {
		Graphics::GetRenderTarget()->BeginDraw();
		Graphics::GetRenderTarget()->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		Graphics::DrawTextF(L"You lost. Press 'Enter' to play again!", 0, Structures::Window::GetHeight() / 2,
			Structures::Window::GetWidth(), 20, Graphics::whiteColor.Get());
		Graphics::GetRenderTarget()->EndDraw();
	}
	void Load() override {
		if (strcmp(SaveSystem::GetFileName(), "Score.txt") == 0)
			SaveSystem::FileInit("Score.txt");
		GameFile::nHighScore = atoi(SaveSystem::ReadLine().c_str());
	}
	void UnLoad() override {

	}
};