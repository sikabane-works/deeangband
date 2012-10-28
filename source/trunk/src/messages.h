
// Keywords

#if JP
static const cptr game_messages[] =
{
	"クエストを達成した。",
	"改良に失敗した。"
	"しかし効力を跳ね返した。",
	"しかし効果がなかった。",
	"クリーチャーが立ちふさがっている。",
	"鍵をはずした。",
	"鍵をはずせなかった。",
	"「なんだこの階段は！？」",
};
#else
static const cptr game_messages[] =
{
	"You completed the quest.",
	"The improvement failed.",
	"You resist the effects.",
	"You are unaffected.",
	"There is a creature in the way.",
	"You have picked the lock.",
	"You failed to pick the lock.",
	"What's a fucking stair!?",
};
#endif

enum GAME_MESSAGE
{
	GAME_MESSAGE_COMPLETE_QUEST,
	GAME_MESSAGE_IMPROVEMENT_FAILED,
	GAME_MESSAGE_RESIST_THE_EFFECT,
	GAME_MESSAGE_IS_UNAFFECTED,
	GAME_MESSAGE_CREATURE_IN_THE_WAY,
	GAME_MESSAGE_SUCCESS_PICKING,
	GAME_MESSAGE_FAILED_PICKING,
	GAME_MESSAGE_COMBAT_TALK_STAIR,
};
