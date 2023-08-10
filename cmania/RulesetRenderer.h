#pragma once
#include "Ruleset.h"
class RulesetRendererBase {
public:
	virtual ~RulesetRendererBase() {
	}
};
/// <summary>
/// ��ʾ��һ�� Ruleset ����Ⱦ��.
/// </summary>
/// <typeparam name="HitObject"></typeparam>
template <typename Ruleset>
class RulesetRenderer : RulesetRendererBase {
public:
	virtual void RenderRuleset(Ruleset& ruleset, GameBuffer& buffer) = 0;
};