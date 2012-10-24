
// Keywords

#if JP
static const cptr game_messages[] =
{
	"‚µ‚©‚µŒø—Í‚ğ’µ‚Ë•Ô‚µ‚½B",
	"‚µ‚©‚µŒø‰Ê‚ª‚È‚©‚Á‚½B",
};
#else
static const cptr game_messages[] =
{
	"You resist the effects",
	"You are unaffected.",
};
#endif

enum GAME_MESSAGE
{
	GAME_MESSAGE_RESIST_THE_EFFECT,
	GAME_MESSAGE_IS_UNAFFECTED,
};
