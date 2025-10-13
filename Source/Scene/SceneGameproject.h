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
class SceneGameproject : public Scene
{
public:
	SceneGameproject() {};
	~SceneGameproject() override {};

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

	Stage* stage = nullptr;

	Player* player = nullptr;

	Sprite* sprite = nullptr;
	Sprite* sprite_number = nullptr;
	Sprite* sprite_text = nullptr;

	Balloon* balloon = nullptr;
	
	Board* board = nullptr;

	Box* box = nullptr;

	CameraController* cameraController = nullptr;
public:

	SceneGameproject(Stage* stage, Player* player, CameraController* cameraController)
		: stage(stage), player(player), cameraController(cameraController)
	{
	}
};
