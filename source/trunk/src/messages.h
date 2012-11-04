
// Keywords

#if JP
static const cptr game_messages[] =
{
	"クエストを達成した。",
	"改良に失敗した。"
	"しかし効力を跳ね返した。",
	"には完全な耐性がある。",
	"には耐性がある。",
	"が効果がなかった。",
	"は混乱している。",
	"クリーチャーが立ちふさがっている。",
	"鍵をはずした。",
	"鍵をはずせなかった。",
	"恐怖の暗黒オーラがあなたの%sを包み込んだ！",
	"「なんだこの階段は！？」",
};
#else
static const cptr game_messages[] =
{
	"You completed the quest.",
	"The improvement failed.",
	"You resist the effects.",
	"is immune.",
	"resists.",
	"You are unaffected.",
	"is confused",
	"There is a creature in the way.",
	"You have picked the lock.",
	"You failed to pick the lock.",
	"A terrible black aura blasts your %s!",
	"'What's a fucking stair!?'",
};
#endif

enum GAME_MESSAGE
{
	GAME_MESSAGE_COMPLETE_QUEST,
	GAME_MESSAGE_IMPROVEMENT_FAILED,
	GAME_MESSAGE_RESIST_THE_EFFECT,
	GAME_MESSAGE_IS_IMMUNE,
	GAME_MESSAGE_RESISTED,
	GAME_MESSAGE_IS_UNAFFECTED,
	GAME_MESSAGE_IS_CONFUSED,
	GAME_MESSAGE_CREATURE_IN_THE_WAY,
	GAME_MESSAGE_SUCCESS_PICKING,
	GAME_MESSAGE_FAILED_PICKING,
	GAME_MESSAGE_BLACK_AURA_TO_OBJECT,
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

static const cptr message_files[]=
{
#ifdef JP
	"a_cursed_j.txt",
	"a_low_j.txt",
	"a_med_j.txt",
	"a_high_j.txt",
	"w_cursed_j.txt",
	"w_low_j.txt",
	"w_med_j.txt",
	"w_high_j.txt",
	"chainswd_j.txt",
#else
	"a_cursed.txt",
	"a_low.txt",
	"a_med.txt",
	"a_high.txt",
	"w_cursed.txt",
	"w_low.txt",
	"w_med.txt",
	"w_high.txt",
	"chainswd.txt",
#endif
};

enum MESSAGE_FILES
{
	MESSAGE_FILES_ARMOUR_CURSED_ARTIFACT,
	MESSAGE_FILES_ARMOUR_LOW_ARTIFACT,
	MESSAGE_FILES_ARMOUR_MED_ARTIFACT,
	MESSAGE_FILES_ARMOUR_HIGH_ARTIFACT,
	MESSAGE_FILES_WEAPON_CURSED_ARTIFACT,
	MESSAGE_FILES_WEAPON_LOW_ARTIFACT,
	MESSAGE_FILES_WEAPON_MED_ARTIFACT,
	MESSAGE_FILES_WEAPON_HIGH_ARTIFACT,
	MESSAGE_FILES_WEAPON_CHAINSWORD,
};