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
class SceneTutorial : public Scene
{
public:
	SceneTutorial() {};
	~SceneTutorial() override {};

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
	Stage* stage = nullptr;

	Player* player = nullptr;

	Sprite* sprite = nullptr;

	Balloon* balloon = nullptr;

	Board* board = nullptr;

	Box* box = nullptr;

	Sprite* sprite2 = nullptr;
	Sprite* sprite3 = nullptr;

	CameraController* cameraController = nullptr;
	int change = 0;
	
public:
	

	SceneTutorial(Stage* stage, Player* player, CameraController* cameraController)
		: stage(stage), player(player), cameraController(cameraController)
	{
	}
};
