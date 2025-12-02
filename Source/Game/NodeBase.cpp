#include "Game/JudgmentBase.h"
#include "Game/NodeBase.h"
#include "Game/EnemySlime.h"
#include "Game/BehaviorData.h"
#include "Game/ActionBase.h"
#include <random>



// デストラクタ
NodeBase::~NodeBase()
{
	delete judgment;
	delete action;
}

// ノード検索
NodeBase* NodeBase::SearchNode(std::string searchName)
{
	// 名前が一致
	if (name == searchName)
	{
		return this;
	}
	else {
		// 子ノードで検索
		for (auto itr = children.begin(); itr != children.end(); itr++)
		{
			NodeBase* ret = (*itr)->SearchNode(searchName);

			if (ret != nullptr)
			{
				return ret;
			}
		}
	}

	return nullptr;
}

// ノード推論
NodeBase* NodeBase::Inference(BehaviorData* data)
{
	std::vector<NodeBase*> list;
	NodeBase* result = nullptr;

	// childrenの数だけループを行う。
	for (int i = 0; i < children.size(); i++)
	{
		// children.at(i)->judgmentがnullptrでなければ
		if (children.at(i)->judgment != nullptr)
		{
			// TODO 05_03
			// children.at(i)->judgment->Judgment()関数を実行し、tureであれば
			// listにchildren.at(i)を追加していく
			if (children.at(i)->Judgment() == true)  // ←自分じゃなくて子のジャッジメントを使う
			{
				list.push_back(children.at(i));
			}
		}
		else {
			// TODO 05_03
			// 判定クラスがなければ無条件に追加
			list.push_back(children.at(i)); // 判定がなければ無条件で実行対象
		}
	}

	// 選択ルールでノード決め
	switch (selectRule)
	{
		// 優先順位
	case BehaviorTree::SelectRule::Priority:
		result = SelectPriority(&list);
		break;
		// ランダム
	case BehaviorTree::SelectRule::Random:
		result = SelectRandom(&list);
		break;
		// シーケンス
	case BehaviorTree::SelectRule::Sequence:
	case BehaviorTree::SelectRule::SequentialLooping:
		result = SelectSequence(&list, data);
		break;
	}

	if (result != nullptr)
	{
		// 行動があれば終了
		if (result->HasAction() == true)
		{
			return result;
		}
		else {
			// 決まったノードで推論開始
			result = result->Inference(data);
		}
	}

	return result;
}

// 優先順位でノード選択
NodeBase* NodeBase::SelectPriority(std::vector<NodeBase*>* list)
{
	NodeBase* selectNode = nullptr;
	int priority = INT_MAX;

	// TODO 05_04
	// 一番優先順位が高いノードを探してselectNodeに格納
	for (auto node : *list)
	{
		if (node->GetPriority() < priority)
		{
			priority = node->GetPriority();
			selectNode = node;
		}
	}

	return selectNode;
}


// ランダムでノード選択
NodeBase* NodeBase::SelectRandom(std::vector<NodeBase*>* list)
{
	if (!list->empty()) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, list->size() - 1);

		int selectNo = dist(gen);
		return list->at(selectNo);
	}
	else {
		return nullptr;
	}
}

// シーケンス・シーケンシャルルーピングでノード選択
NodeBase* NodeBase::SelectSequence(std::vector<NodeBase*>* list, BehaviorData* data)
{
	int step = 0;

	// 指定されている中間ノードのシーケンスがどこまで実行されたか取得する
	step = data->GetSequenceStep(name);

	// 中間ノードに登録されているノード数以上の場合、
	if (step >= children.size())
	{
		// TODO 05_06
		// ルールによって処理を切り替える
		// ルールがBehaviorTree::SelectRule::SequentialLoopingのときは最初から実行するため、stepに0を代入
		// ルールがBehaviorTree::SelectRule::Sequenceのときは次に実行できるノードがないため、nullptrをリターン
		if (selectRule == BehaviorTree::SelectRule::SequentialLooping)
		{
			step = 0;
		}
		else if (selectRule == BehaviorTree::SelectRule::Sequence)
		{
			return nullptr;
		}
	}
	// 実行可能リストに登録されているデータの数だけループを行う
	for (auto itr = list->begin(); itr != list->end(); itr++)
	{
		// 子ノードが実行可能リストに含まれているか
		if (children.at(step)->GetName() == (*itr)->GetName())
		{
			// TODO 05_07
			// 現在の実行ノードの保存、次に実行するステップの保存を行った後、
			// 現在のステップ番号のノードをリターンしなさい
			// ①スタックにはdata->PushSequenceNode関数を使用する。保存するデータは実行中の中間ノード。
			// ②また、次に実行する中間ノードとステップ数を保存する
			// 　保存にはdata->SetSequenceStep関数を使用。
			// 　保存データは中間ノードの名前と次のステップ数です(step + 1)
			// ③ステップ番号目の子ノードを実行ノードとしてリターンする
			data->PushSequenceNode(this);
			data->SetSequenceStep(name, step + 1);
			return children.at(step);

		}
	}
	// 指定された中間ノードに実行可能ノードがないのでnullptrをリターンする
	return nullptr;
}

// 判定
bool NodeBase::Judgment()
{
	// TODO 05_08
	// judgmentがあるか判断。あればメンバ関数Judgment()実行した結果をリターン。
	if (judgment != nullptr)
	{
		return judgment->Judgment();
	}


	return true;
}

// ノード実行
ActionBase::State NodeBase::Run(float elapsedTime)
{
	// TODO 05_09
	// actionがあるか判断。あればメンバ関数Run()実行した結果をリターン。
	if (action != nullptr)
	{
		return action->Run(elapsedTime);
	}

	else
		return ActionBase::State::Failed;
}
