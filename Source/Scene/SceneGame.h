#pragma once

#include "../Game/Stage.h"
#include "../Game/Player.h"
#include "CameraController.h"
#include "DirectXMath.h"
#include "System/Sprite.h"
#include "Scene.h"
#include "../Game/Balloon.h"
#include "../Game/Board.h"
#include "../Game/Box.h"

// �Q�[���V�[��
class SceneGame : public Scene
{
public:
	SceneGame() {};
	~SceneGame() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

	// GUI�`��
	void DrawGUI() override;

private:
	float game_timer;

	std::unique_ptr<Stage> stage = nullptr;
	
	std::unique_ptr<Player> player = nullptr;

	std::unique_ptr<Sprite> sprite = nullptr;
	std::unique_ptr<Sprite> sprite_number = nullptr;
	std::unique_ptr<Sprite> sprite_text = nullptr;

	Balloon* balloon = nullptr;
	
	Board* board = nullptr;

	Box* box = nullptr;

	CameraController* cameraController = nullptr;

public:
	static void RoadModel();
};
