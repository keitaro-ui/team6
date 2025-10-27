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
	std::unique_ptr<Stage> stage = nullptr;

	std::unique_ptr<Player> player = nullptr;

	Balloon* balloon = nullptr;

	Board* board = nullptr;

	Box* box = nullptr;

	std::unique_ptr<Sprite> sprites[3];
	
	CameraController* cameraController = nullptr;
	int change = 0;
};
