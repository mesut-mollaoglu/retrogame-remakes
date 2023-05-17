#pragma once
#include "SaveSystem.h"

#define nScreenWidth 600
#define nScreenHeight 600
#define nMaxHorizontal 7
#define nMaxVertical 15
#define nMaxSize 105

class Tetris {
public:
	typedef struct Matrix {
		std::vector<std::vector<float>> mat;
		Matrix() = default;
		Matrix(std::vector<std::vector<float>> arr){
			mat = arr;
		}
		void RotateClockWise() {
			for (int i = 0; i < 2; i++)
			{
				for (int j = i; j < 3 - i; j++)
				{
					float temp = mat[i][j];
					mat[i][j] = mat[3 - j][i];
					mat[3 - j][i] = mat[3 - i][3 - j];
					mat[3 - i][3 - j] = mat[j][3 - i];
					mat[j][3 - i] = temp;
				}
			}
		}
		void RotateCounterClockWise() {
			for (int i = 0; i < 2; i++)
			{
				for (int j = i; j < 3 - i; j++)
				{
					int temp = mat[i][j];
					mat[i][j] = mat[j][3 - i];
					mat[j][3 - i] = mat[3 - i][3 - j];
					mat[3 - i][3 - j] = mat[3 - j][i];
					mat[3 - j][i] = temp;
				}
			}
		}
	}Matrix;
	static Matrix Tetrominos[7];
	static Math::float3 colors[8];
	struct Node {
		ComPtr<ID3D11Buffer> vertexBuffer;
		ComPtr<ID3D11Buffer> indexBuffer;
		ComPtr<ID3D11Buffer> constantBuffer;
		Math::float2 position;
		Math::float2 nOffset;
		bool bIsGrounded = false;
		void InitBuffers(Math::float3 color) {
			Graphics::Vertex data[] =
			{
				XMFLOAT2(-0.5f, -0.5f), XMFLOAT4(color.x, color.y, color.z, 1.0f), XMFLOAT2(0, 1),
				XMFLOAT2(-0.5f, 0.5f), XMFLOAT4(color.x, color.y, color.z, 1.0f), XMFLOAT2(0, 0),
				XMFLOAT2(0.5f, 0.5f), XMFLOAT4(color.x, color.y, color.z, 1.0f), XMFLOAT2(1, 0),
				XMFLOAT2(0.5f, -0.5f), XMFLOAT4(color.x, color.y, color.z, 1.0f), XMFLOAT2(1, 1)
			};
			Graphics::CreateVertexBuffer(vertexBuffer, data, ARRAYSIZE(data));
			Graphics::CreateIndexBuffer(indexBuffer);
			Graphics::CreateConstantBuffer<Graphics::Constants>(constantBuffer);
		}
		Node() = default;
		Node(Math::float2 pos) {
			srand(time(0));
			position = pos;
			nOffset = Math::float2();
			InitBuffers(Tetris::colors[Math::randInt(0, 8)]);
		}
		void Update() {
			if (nOffset != Math::float2()) {
				position += nOffset;
				nOffset = Math::float2();
			}
			if (position.y <= (-Math::floor(Graphics::GetEyeDistance().z) * nScreenHeight))
				bIsGrounded = true;
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
	};
	struct Tetromino{
		Math::float2 position;
		Math::float2 nOffset;
		Tetris::Matrix matrix;
		std::vector<Node*> nodes;
		int nTimePassed = 0;
		Tetromino() = default;
		bool bDrop = false;
		bool bIsGrounded = false;
		void BuildTetromino() {
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					if (matrix.mat[i][j] == 1)
						nodes.push_back(new Node(Math::float2(i * nScreenWidth, j * nScreenHeight)));
		}
		void RotateTetromino(int nRotate = 1) {
			(nRotate == 1) ? matrix.RotateClockWise() : matrix.RotateCounterClockWise();
			int count = 0;
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					if (matrix.mat[i][j] == 1) {
						nodes[count]->position = position + Math::float2(i * nScreenWidth, j * nScreenHeight);
						count++;
					}
		}
		Tetromino(std::vector<std::vector<float>> arr) {
			nOffset = Math::float2();
			position = Math::float2(Math::randInt(0, 4) * nScreenWidth, 
				nScreenHeight * (Math::floor(Graphics::GetEyeDistance().z) - 2));
			position -= Math::float2(2 * nScreenWidth, 0);
			matrix = Matrix(arr);
			BuildTetromino();
			for (Node* node : nodes)
				node->position += position;
		}
		void Render() {
			if (nodes.empty())
				return;
			for (Node* node : nodes)
				node->Render();
		}
		void Update() {
			nTimePassed++;
			if (!bIsGrounded && BaseStateMachine::isKeyPressed('W')) {
				this->RotateTetromino(1);
				for (Node* node : Tetris::vGrounded)
					for (Node* cNode : nodes)
						if (cNode->position == node->position)
							this->RotateTetromino(0);
			}
			if (BaseStateMachine::isKeyPressed('D')) {
				int eOffset = nOffset.x;
				if (position.x < Graphics::GetEyeDistance().z * Structures::Window::GetWidth())
					nOffset.x += nScreenWidth;
				for (Node* cNode : this->nodes)
					for (Node* node : Tetris::vGrounded)
						if (cNode->position.x + nScreenWidth == node->position.x && cNode->position.y == node->position.y && eOffset != nOffset.x)
							nOffset.x -= nScreenWidth;
			}
			if (BaseStateMachine::isKeyPressed('A')) {
				int eOffset = nOffset.x;
				if(position.x > -Graphics::GetEyeDistance().z * Structures::Window::GetWidth())
					nOffset.x -= nScreenWidth;
				for (Node* cNode : this->nodes)
					for (Node* node : Tetris::vGrounded)
						if (cNode->position.x - nScreenWidth == node->position.x && cNode->position.y == node->position.y && eOffset != nOffset.x)
							nOffset.x += nScreenWidth;
			}
			if (BaseStateMachine::isKeyPressed('Q')) {
				bDrop = true;
			}
			if (!bIsGrounded) {
				if (bDrop)
					nOffset.y -= nScreenHeight;
				else if (nTimePassed % 6 == 0)
					nOffset.y -= nScreenHeight;
			}
			position += nOffset;
			for (Node* node : nodes) {
				node->nOffset += nOffset;
			}
			nOffset = Math::float2();
		}
		void FixedUpdate() {
			for (Node* node : this->nodes) {
				node->Update();
				if (node->bIsGrounded) {
					this->bIsGrounded = true;
				}
			}
			for (Node* cNode : this->nodes) {
				for (Node* node : Tetris::vGrounded) {
					if (cNode->position.y - nScreenHeight == node->position.y && cNode->position.x == node->position.x) {
						this->bIsGrounded = true;
					}
				}
			}
		}
	};
	static void ClearBoard() {
		for (int i = 0; i < nMaxSize; i++) {
			Tetris::board[i] = 0;
		}
	}
	static void FillBoard() {
		for (Node* node : vGrounded) {
			int xCoord = node->position.x / nScreenWidth + 3;
			int yCoord = node->position.y / nScreenHeight + 7;
			Tetris::board[yCoord * nMaxHorizontal + xCoord] = 1;
		}
	}
	static void ReadBoard() {
		for (int i = 0; i < nMaxSize; i+=nMaxHorizontal) {
			bool bLineFill = true;
			for (int j = i; j < i + nMaxHorizontal; j++) {
				bLineFill &= (board[j] == 1);
			}
			if (bLineFill) {
				for (int j = i; j < i + nMaxHorizontal; j++) {
					for (int k = 0; k < vGrounded.size(); k++) {
						int yCoord = (i / nMaxHorizontal) * nScreenHeight - 4200;
						int xCoord = (j % nMaxHorizontal) * nScreenWidth - 1800;
						if (vGrounded[k]->position == Math::float2(xCoord, yCoord)) {
							vTemp.push_back(vGrounded[k]);
							vGrounded.erase(vGrounded.begin() + k);
						}
						else if (vGrounded[k]->position.y > yCoord)
							vGrounded[k]->nOffset.y = -nScreenHeight;
					}
				}
				Tetris::nScore += (1 << int(i / nMaxHorizontal)) * 100;
			}
		}
		for (Node* node : vTemp)
			delete node;
		vTemp.clear();
	}
	static int nScore;
	static int nHighScore;
	static std::vector<Node*> vGrounded;
	static std::vector<Node*> vTemp;
	static int board[nMaxSize];
};

class GameLoop : public Level {
public:
	GameLoop() {
		nLevel = Level::ManageLevel::CurrentLevel;
		ticks = 50;
	}
	void Render() override {
		using Structures::Camera;
		Graphics::SetConstantValues<Graphics::ProjectionBuffer>(Graphics::projectionBuffer.Get(), {
			Camera::projMatrix, Camera::worldMatrix, Camera::viewMatrix });
		Graphics::Clear(0.5f, 0.5f, 0.5f, 1.0f);
		Graphics::Begin();
		Sleep(this->ticks);
		vTetrominos.back()->Render();
		for (Tetris::Node* nGrounded : Tetris::vGrounded)
			nGrounded->Render();
		Graphics::End();
	}
	void Update() override {
		for (Tetris::Node* nGrounded : Tetris::vGrounded)
			nGrounded->Update();
		vTetrominos.back()->Update();
	}
	void FixedUpdate() override {
		Tetris::ClearBoard();
		Tetris::FillBoard();
		if (vTetrominos.back()->bIsGrounded) {
			Tetris::nScore += 25;
			for (Tetris::Node* node : vTetrominos.back()->nodes) {
				Tetris::vGrounded.push_back(node);
			}
			vTetrominos.push_back(new Tetris::Tetromino(Tetris::Tetrominos[Math::randInt(0, 7)].mat));
			delete vTetrominos[0];
			vTetrominos.erase(vTetrominos.begin());
		}
		vTetrominos.back()->FixedUpdate();
		Tetris::ReadBoard();
		for(Tetris::Node* node : Tetris::vGrounded)
			if (node->position.y >= nScreenHeight * Math::floor(Graphics::GetEyeDistance().z)) {
				if (Tetris::nScore > Tetris::nHighScore) {
					SaveSystem::ClearCurrentFile();
					SaveSystem::WriteData(std::to_string(Tetris::nScore));
				}
				this->nLevel = Level::ManageLevel::NextLevel;
			}
	}
	void Load() override {
		Tetris::nScore = 0;
		Tetris::vGrounded.clear();
		vTetrominos.clear();
		vTetrominos.push_back(new Tetris::Tetromino(Tetris::Tetrominos[Math::randInt(0, 7)].mat));
	}
	void UnLoad() override {
		vTetrominos.clear();
		Tetris::vGrounded.clear();
	}
private:
	std::vector<Tetris::Tetromino*> vTetrominos;
};

class TetrisGameOver : public Level {
public:
	TetrisGameOver() {
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
		Graphics::DrawTextF(L"Score: " + std::to_wstring(Tetris::nScore), 0,
			Structures::Window::GetHeight() / 2 - 100, Structures::Window::GetWidth(), 20, Graphics::whiteColor.Get());
		Graphics::DrawTextF(L"You lost. Press 'Enter' to play again!", 0, Structures::Window::GetHeight() / 2 + 100,
			Structures::Window::GetWidth(), 20, Graphics::whiteColor.Get());
		Graphics::GetRenderTarget()->EndDraw();
	}
	void Load() override {
		if (strcmp(SaveSystem::GetFileName(), "Score.txt") == 0)
			SaveSystem::FileInit("Score.txt");
		Tetris::nHighScore = atoi(SaveSystem::ReadLine().c_str());
	}
	void UnLoad() override {
		Tetris::nScore = 0;
	}
};

class TetrisMainMenu : public Level {
public:
	TetrisMainMenu() {
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
		Graphics::DrawTextF(L"High Score: " + std::to_wstring(Tetris::nHighScore), 0,
			Structures::Window::GetHeight() / 2, Structures::Window::GetWidth(), 20, Graphics::whiteColor.Get());
		Graphics::DrawTextF(L"Press 'Enter' to play!", 0, Structures::Window::GetHeight() / 2 - 200,
			Structures::Window::GetWidth(), 20, Graphics::whiteColor.Get());
		Graphics::GetRenderTarget()->EndDraw();
	}
	void Load() override {
		if (strcmp(SaveSystem::GetFileName(), "Score.txt") == 0)
			SaveSystem::FileInit("Score.txt");
		Tetris::nHighScore = atoi(SaveSystem::ReadLine().c_str());
	}
	void UnLoad() override {

	}
};