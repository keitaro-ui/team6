#pragma once
#include "Scene.h"
#include <string>
#include "System/RenderContext.h"
#include "../Source/System/Sprite.h"
#include <memory>
#include <chrono>
#include "System/Mouse.h"

class ScenePassword 
{
public:


    ScenePassword(const std::string& correctPass);
    ~ScenePassword();
    void Initialize();
    void Update(float elapsedTime);
    void Render();

    bool IsFinished() const { return finished; }
    bool IsCorrect() const { return correct; }

private:
    void OnMouseDown(float mx, float my);
    void OnClickOKButton();

private:
    struct TriRect 
    {
        float x, y;
        float w, h;
    };

    struct ButtonRect 
    {
        float x;
		float y;
        float w;
		float h;
	};

    TriRect tri[4];  // 4つ分

    ButtonRect okButtonRect;

    std::string correctPassword;   // 正解パス
    std::string inputPassword;     // 入力中のパス

    bool wrong = false;            // 間違い表示
    bool transitionDone = false;   // 遷移一度だけ

	bool isAnswerReady = false; // 回答が揃ったかどうか

    std::unique_ptr<Sprite> Password;
	std::unique_ptr<Sprite> PassBackGround;
	std::unique_ptr<Sprite> Triangle;
	std::unique_ptr<Sprite> OK;

    float darkAmount = 0;

    float clickNum[4];

    float screenWidth = 0.0f;
    float screenHeight = 0.0f;
    
    float cooltime;

    float textureWidth = 3725.0f;
    float textureHeight = 514.0f;
    float digitWidth = textureWidth / 10.0f;
    float digitHeight = textureHeight;

    bool finished = false;
    bool correct = false;
};