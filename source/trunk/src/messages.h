
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

// Hack -- possible "insult" messages
static const cptr desc_insult[] =
{
#ifdef JP
	"があなたを侮辱した！",
	"があなたの母を侮辱した！",
	"があなたを軽蔑した！",
	"があなたを辱めた！",
	"があなたを汚した！",
	"があなたの回りで踊った！",
	"が猥褻な身ぶりをした！",
	"があなたをぼんやりと見た！！！",
	"があなたをパラサイト呼ばわりした！",
	"があなたをサイボーグ扱いした！"
#else
	"insults you!",
	"insults your mother!",
	"gives you the finger!",
	"humiliates you!",
	"defiles you!",
	"dances around you!",
	"makes obscene gestures!",
	"moons you!!!"
	"calls you a parasite!",
	"calls you a cyborg!"
#endif
};

// Hack -- possible "insult" messages
static const cptr desc_moan[] =
{
#ifdef JP
	"は何かを悲しんでいるようだ。",
	"が彼の飼い犬を見なかったかと尋ねている。",
	"が縄張りから出て行けと言っている。",
	"はキノコがどうとか呟いている。"
#else
	"seems sad about something.",
	"asks if you have seen his dogs.",
	"tells you to get off his land.",
	"mumbles something about mushrooms."
#endif
};


