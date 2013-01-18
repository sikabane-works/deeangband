
// Keywords

#ifdef JP
#define KEYWORD_CURSED "呪われている"
#else
#define KEYWORD_CURSED "cursed"
#endif


#ifdef JP

#define SYS_MESSAGE_FAILED_TEMPFILE "一時ファイル %s を作成できませんでした。"
#define SYS_MESSAGE_FAILED_FILEOPEN "ファイルを開けません。"
#define SYS_MESSAGE_FAILED_FILEOPEN2 "%s を開くことができませんでした。"
#define SYS_MESSAGE_REPLACE_FILE "現存するファイルに上書きしますか? (%s)"
#define SYS_MESSAGE_HIT_ANY_KEY "何かキーを押して下さい。"
#define SYS_MESSAGE_OPTION_AUX "初期オプション((*)はスコアに影響)"
#define SYS_MESSAGE_OPTION_AUX2 "初期オプション(参照のみ)"
#define SYS_MESSAGE_SAVE_FAILED "セーブ失敗！"
#define SYS_MESSAGE_SCORE_FAILED "スコア・ファイルが使用できません。"
#define SYS_MESSAGE_PANIC_SAVE_SUCCEED "緊急セーブ成功！"
#define SYS_MESSAGE_PANIC_SAVE_FAILED "緊急セーブ失敗！"
#define SYS_MESSAGE_YOUR_TRAITS "あなたには以下の特性がある"
#define SYS_MESSAGE_YOUR_NO_TRAIT "あなたには特別な特性がない"
#define SYS_MESSAGE_SERVANTS "%sには%d種類、%d体の従僕がいる"
#define SYS_MESSAGE_NO_SERVANT "%sには従僕がいない"
#define SYS_MESSAGE_UNKNOWN_SYMBOL "無効な文字"
#define SYS_MESSAGE_ERROR "エラー"
#define SYS_MESSAGE_OUT_OF_SWITCH "switch構文範囲外"

#define SYS_MESSAGE_TODAY "今日"
#define SYS_MESSAGE_NORTH "北"
#define SYS_MESSAGE_SOUTH "南"
#define SYS_MESSAGE_EAST "東"
#define SYS_MESSAGE_WEST "西"
#define SYS_MESSAGE_MONEY "所持金"
#define SYS_MESSAGE_WEIGHT "重量"

#define SYS_MESSAGE_MODIFICATION "能力修正"

#define SYS_MESSAGE_VISUAL_COMMAND "コマンド (n/N/^N/a/A/^A/c/C/^C/v/V/^V): "
#define SYS_MESSAGE_ASK_USE "%sを使いますか？"

#else

#define SYS_MESSAGE_FAILED_TEMPFILE "Failed to create temporary file %s."
#define SYS_MESSAGE_FAILED_FILEOPEN "Can not open file."
#define SYS_MESSAGE_FAILED_FILEOPEN2 "Failed to open %s."
#define SYS_MESSAGE_REPLACE_FILE "Replace existing file %s? "
#define SYS_MESSAGE_HIT_ANY_KEY "Hit Any Key。"
#define SYS_MESSAGE_OPTION_AUX "Birth Option((*)s effect score)"
#define SYS_MESSAGE_OPTION_AUX2 "Birth Options(browse only)"
#define SYS_MESSAGE_SAVE_FAILED "death save failed!"
#define SYS_MESSAGE_SCORE_FAILED "Score file unavailable."
#define SYS_MESSAGE_PANIC_SAVE_SUCCEED "Panic save succeeded!"
#define SYS_MESSAGE_PANIC_SAVE_FAILED "panic save failed!"
#define SYS_MESSAGE_YOUR_TRAITS "You have these trait..."
#define SYS_MESSAGE_YOUR_NO_TRAIT "You have no trait"
#define SYS_MESSAGE_YOUR_SERVANT "%s have no servant"
#define SYS_MESSAGE_YOUR_NO_SERVANT "%s have %d species, %d servants"
#define SYS_MESSAGE_UNKNOWN_SYMBOL "unknown symbol"
#define SYS_MESSAGE_ERROR "Error"
#define SYS_MESSAGE_OUT_OF_SWITCH "Out of switch."
#define SYS_MESSAGE_TODAY "Today"
#define SYS_MESSAGE_NORTH " North"
#define SYS_MESSAGE_SOUTH " South"
#define SYS_MESSAGE_EAST " East"
#define SYS_MESSAGE_WEST " West"
#define SYS_MESSAGE_MONEY "AU"
#define SYS_MESSAGE_WEIGHT "Weight"

#define SYS_MESSAGE_MODIFICATION "Modification"

#define SYS_MESSAGE_VISUAL_COMMAND "Command (n/N/^N/a/A/^A/c/C/^C/v/V/^V):"
#define SYS_MESSAGE_ASK_USE "use %s?"

#endif
 
#if JP
#define KW_CREATURE "クリーチャー"
#define KW_NAME "名前"
#define KW_RACE "種族"
#define KW_CLASS "職業"
#define KW_CHARA "性格"
#define KW_PATLON "主神"
#define KW_HP "HP"
#define KW_MP "MP"
#define KW_FAIL "失率"
#define KW_UNKNOWN "未知"
#define KW_FORGOTTEN "忘却"
#define KW_ILLEGIBLE "判読不能"
#define KW_UNTRIED "未経験"
#define KW_CARRYING_WEIGHT "所持重量"
#define KW_EQUIPMENT_WEIGHT "装備重量"
#define KW_STAFF "杖"
#define KW_WAND "魔法棒"
#define KW_ROD "ロッド"
#define KW_YOU "あなた"
#define KW_CREATURE_PROFILE "クリーチャーのプロフィール"
#define KW_GOOD "善"
#define KW_EVIL "悪"
#define KW_LAW "秩序"
#define KW_CHAOS "混沌"
#define KW_BALANCE "天秤"
#define KW_DAM "損傷"
#define KW_RAN "射程"
#define KW_POW "効力"
#define KW_RAD "半径"
#define KW_YES "はい"
#define KW_NO "いいえ"
#define KW_NONE "なし"
#define KW_SURFACE "地上"
#define KW_SCORE_RANKING "勇者の殿堂"
#define KW_MAX "最高"
#else
#define KW_CREATURE "Creatures"
#define KW_NAME "Name"
#define KW_RACE "Race"
#define KW_CLASS "Class"
#define KW_CHARA "Chara"
#define KW_PATLON "Patron"
#define KW_HP "HP"
#define KW_MP "MP"
#define KW_FAIL "Fail"
#define KW_UNKNOWN "Unknown"
#define KW_FORGOTTEN "Forgotten"
#define KW_ILLEGIBLE "Illegible"
#define KW_UNTRIED "Untried"
#define KW_CARRYING_WEIGHT "Carrying Weight"
#define KW_EQUIPMENT_WEIGHT "Equipment Weight"
#define KW_STAFF "staff"
#define KW_WAND "wand"
#define KW_ROD "rod"
#define KW_YOU "You"
#define KW_CREATURE_PROFILE "Creature Profile"
#define KW_GOOD "Good"
#define KW_EVIL "Evil"
#define KW_LAW "Law"
#define KW_CHAOS "Chaos"
#define KW_BALANCE "Balance"
#define KW_DAM "Dam"
#define KW_RAN "Ran"
#define KW_POW "Pow"
#define KW_RAD "Rad"
#define KW_YES "Yes"
#define KW_NO "No"
#define KW_NONE "None"
#define KW_SURFACE "the surface"
#define KW_SCORE_RANKING "Hall of Fame"
#define KW_MAX "Max"
#endif

#ifdef JP
#define GAME_MESSAGE_TRAP_BLIND "黒いガスに包み込まれた！"
#define GAME_MESSAGE_TRAP_CONFUSE "きらめくガスに包み込まれた！"
#define GAME_MESSAGE_TRAP_POISON "刺激的な緑色のガスに包み込まれた！"
#define GAME_MESSAGE_TRAP_SLEPT "奇妙な白い霧に包まれた！"
#else
#define GAME_MESSAGE_TRAP_BLIND "A black gas surrounds you!"
#define GAME_MESSAGE_TRAP_CONFUSE "A gas of scintillating colors surrounds you!"
#define GAME_MESSAGE_TRAP_POISON "A pungent green gas surrounds you!"
#define GAME_MESSAGE_TRAP_SLEPT "A strange white mist surrounds you!"
#endif

#if JP
#define GAME_MESSAGE_COMPLETE_QUEST "クエストを達成した。"
#define GAME_MESSAGE_IMPROVEMENT_FAILED "改良に失敗した。"
#define GAME_MESSAGE_RESIST_THE_EFFECT "しかし効力を跳ね返した。"
#define GAME_MESSAGE_IS_IMMUNE "には完全な耐性がある。"
#define GAME_MESSAGE_RESISTED "には耐性がある。"
#define GAME_MESSAGE_IS_UNAFFECTED "には効果がなかった。"
#define GAME_MESSAGE_NO_EFFECT "効果がなかった。"
#define GAME_MESSAGE_IS_CONFUSED "は目が見えない"
#define GAME_MESSAGE_IS_BLIND "は混乱している。"
#define GAME_MESSAGE_CREATURE_IN_THE_WAY "クリーチャーが立ちふさがっている。"
#define GAME_MESSAGE_MANA_RECOVERLY "頭がハッキリとした。"
#define GAME_MESSAGE_SUCCESS_PICKING "鍵をはずした。"
#define GAME_MESSAGE_FAILED_PICKING "鍵をはずせなかった。"
#define GAME_MESSAGE_FOUND_SOMETHING "何かを発見した！"

#define GAME_MESSAGE_BREAK_P_RUNE "守りのルーンが壊れた！"
#define GAME_MESSAGE_BREAK_E_RUNE "ルーンが爆発した！"
#define GAME_MESSAGE_DISARM_E_RUNE "爆発のルーンは解除された。"

#define GAME_MESSAGE_HEAR_NOISE "何かが聞こえた。"

#define GAME_MESSAGE_NO_STUDY_SPELL "読める呪文がない。"
#define GAME_MESSAGE_NO_STUDY_TECH "学べる技がない。"

#define GAME_MESSAGE_BLACK_AURA_TO_OBJECT "恐怖の暗黒オーラがあなたの%sを包み込んだ！"
#define GAME_MESSAGE_REMOVED_OBJECT_CURSE "誰かに見守られているような気がする。"
#define GAME_MESSAGE_COMBAT_TALK_STAIR "「なんだこの階段は！？」"
#define GAME_MESSAGE_PERILOUS_SECRET "石を制御できない。"
#define GAME_MESSAGE_RESISTED_FEAR "しかし恐怖に侵されなかった。"
#define GAME_MESSAGE_NO_MONEY "お金が足りません。"
#define GAME_MESSAGE_NO_MONEY_INSULT "おい！金がないじゃないか！出ていけ！"
#define GAME_MESSAGE_BOUNTY_1 "%s を換金しますか？"

#define GAME_MESSAGE_TRAP_WARNING "* 注意:この先はトラップの感知範囲外です！ *"
#define GAME_MESSAGE_TRAP_DESTROY "トラップを粉砕した。"

#define GAME_MESSAGE_LITE_FAINT "明かりが微かになってきている。"
#define GAME_MESSAGE_LITE_OUT "明かりが消えてしまった！"

#define GAME_MESSAGE_FAILED_RUNAWAY "うまく逃げられなかった。"

#define GAME_MESSAGE_TRAP_NEEDLE "仕掛けられていた小さな針に刺されてしまった！"
#define GAME_MESSAGE_TRAP_DARTS "小さなダーツが飛んできて刺さった！"
#define GAME_MESSAGE_TRAP_DARTS_MISSED "小さなダーツが飛んできた！が、運良く当たらなかった。"
#define GAME_MESSAGE_TRAP_FIRE "炎に包まれた！"
#define GAME_MESSAGE_TRAP_ACID "酸が吹きかけられた！"
#define GAME_MESSAGE_TRAP_ACID_FLOW "突然床から酸が溢れ出した！"
#define GAME_MESSAGE_TRAP_POISON_FLOW "突然床から毒液が溢れ出した！"
#define GAME_MESSAGE_TRAP_PIRANHAS "突然壁から水が溢れ出した！ピラニアがいる！"

#define GAME_MESSAGE_TRAP_POISONOUS_GAS "突如吹き出した緑色のガスに包み込まれた！"
#define GAME_MESSAGE_TRAP_PARALYZE_GAS "突如吹き出した黄色いガスに包み込まれた！"
#define GAME_MESSAGE_TRAP_SUMMONING "突如吹き出した煙に包み込まれた！"
#define GAME_MESSAGE_TRAP_S_ELEMENTAL "宝を守るためにエレメンタルが現れた！"
#define GAME_MESSAGE_TRAP_S_BIRD "鳥の群れがあなたを取り巻いた！"
#define GAME_MESSAGE_TRAP_S_H_DEMON "炎と硫黄の雲の中に悪魔が姿を現した！"
#define GAME_MESSAGE_TRAP_S_H_DRAGON "暗闇にドラゴンの影がぼんやりと現れた！"
#define GAME_MESSAGE_TRAP_S_HYBRID "奇妙な姿の怪物が襲って来た！"
#define GAME_MESSAGE_TRAP_S_VORTEX "渦巻が合体し、破裂した！"
#define GAME_MESSAGE_TRAP_E_RUNE "恐ろしい声が響いた:  「暗闇が汝をつつまん！」"
#define GAME_MESSAGE_TRAP_EXPLOSIVE "突然、箱が爆発した！箱の中の物はすべて粉々に砕け散った！"
#define GAME_MESSAGE_TRAP_SCATTER "宝箱の中身はダンジョンじゅうに散乱した！"
#define GAME_MESSAGE_TRAP_TELEPORT "テレポート・トラップにひっかかった！"
#define GAME_MESSAGE_TRAP_TY_CURSE "何かがピカッと光った！"
#define GAME_MESSAGE_TRAP_ARMAGEDDON "突然天界の戦争に巻き込まれた！"
#define GAME_MESSAGE_TRAP_ALARM "けたたましい音が鳴り響いた！"

#define GAME_MESSAGE_DAMAGE_DENSITY "体の分子が分解した気がする！"
#define GAME_MESSAGE_ROCK_CRUSHING "崩れた岩に押し潰された！"

#define GAME_MESSAGE_SEEM_TO_CURSE "ふーむ、どうやら呪われているようだ。"
#define GAME_MESSAGE_PREVENT_BY_RIDING "乗馬中には出来ない。"
#define GAME_MESSAGE_PREVENT_BY_CONFUSION "混乱状態で上手く出来ない。"
#define GAME_MESSAGE_PREVENT_BY_BLIND "眼が見えなくて出来ない。"
#define GAME_MESSAGE_PREVENT_BY_NO_LITE "明かりがなくて出来ない。"
#define GAME_MESSAGE_PREVENT_BY_HALLUCINATION "幻覚に阻まれてできない。"
#define GAME_MESSAGE_PREVENT_BY_STUNED "頭が朦朧としていて集中できない。"
#define GAME_MESSAGE_PREVENT_BY_PET "今はペットを操ることに集中していないと。"
#define GAME_MESSAGE_PREVENT_BY_NO_MIRROR "鏡の上でないと集中できない。"
#define GAME_MESSAGE_PREVENT_BY_NO_BAREBAND "素手じゃないとできません。"
#define GAME_MESSAGE_PREVENT_BY_HP "HPが足りません."
#define GAME_MESSAGE_PREVENT_BY_MP "MPが足りません。"
#define GAME_MESSAGE_PREVENT_BY_LEVEL "この能力を使用するにはレベル %d に達していなければなりません。"
#define GAME_MESSAGE_PREVENT_BY_ANTIMAGIC "反魔法バリアが魔法を邪魔した！"
#define GAME_MESSAGE_PREVENT_BY_TIMESTOP "止まった時の中ではうまく働かないようだ。"
#define GAME_MESSAGE_ATTEMPT_ANYWAY "それでも挑戦しますか? "

#define GAME_MESSAGE_IMPROVE_LIMIT "そのアイテムはこれ以上改良できない。"

#define GAME_MESSAGE_NO_HAPPEN "何も起こらなかった。"
#define GAME_MESSAGE_RIDING_FAILED "うまく乗れなかった。"
#define GAME_MESSAGE_CONVERT_FAILED "変換に失敗した。"
#define GAME_MESSAGE_NO_DICRECTION_CREATURE "その方向にはクリーチャーはいません。"
#define GAME_MESSAGE_FALL_RIDING "乗騎から地面に落とされた。"

#define GAME_MESSAGE_FIND_TRAP "トラップを発見した。"
#define GAME_MESSAGE_FIND_DOOR "隠しドアを発見した。"

#define GAME_MESSAGE_TELEPORT_PREVENTED "不思議な力がテレポートを防いだ！"

#define GAME_MESSAGE_PATTERN_LIMIT1 "パターンの上を歩き始めると、全てを歩かなければなりません。いいですか？"
#define GAME_MESSAGE_PATTERN_LIMIT2 "パターンの上を歩くにはスタート地点から歩き始めなくてはなりません。"
#define GAME_MESSAGE_PATTERN_LIMIT3 "パターンの上は正しい順序で歩かねばなりません。"
#define GAME_MESSAGE_PATTERN_LIMIT4 "パターンを踏み外してはいけません。"
#define GAME_MESSAGE_PATTERN_GOAL "「パターン」のこの部分は他の部分より強力でないようだ。"
#define GAME_MESSAGE_PATTERN_WRECKED "パターンを血で汚してしまった！何か恐ろしい事が起こりそうだ！"

#define GAME_MESSAGE_ARENA_LIMIT "アリーナが魔法を吸収した！"
#define GAME_MESSAGE_ARENA_BERSERK "狂戦士化していて頭が回らない！"

#define GAME_MESSAGE_SEE_OBJECT "%sがある。"
#define GAME_MESSAGE_GET_MONEY " $%ld の価値がある%sを見つけた。"
#define GAME_MESSAGE_PACK_NO_ROOM "ザックには%sを入れる隙間がない。"
#define GAME_MESSAGE_PACK_NO_ROOM_FLOOR "ザックには床にあるどのアイテムも入らない。"
#define GAME_MESSAGE_PACK_OVERFLOW "ザックからアイテムがあふれた！"
#define GAME_MESSAGE_PACK_REORDER "ザックの中のアイテムを並べ直した。"
#define GAME_MESSAGE_PACK_COMBINE "ザックの中のアイテムをまとめ直した。"
#define GAME_MESSAGE_STAFF_UNSTACK "杖をまとめなおした。"

#define GAME_MESSAGE_UMBUSHED "襲撃だ！"

#define GAME_MESSAGE_PROJECTILE_HITS "%sが%sに命中した。"

#define GAME_MESSAGE_FIELD_MAGIC_FAILED1 "床上のアイテムが呪文を跳ね返した。"

#define GAME_MESSAGE_SMITH_USE_ESSENCE "エッセンスを%d個使用します。"
#define GAME_MESSAGE_SMITH_NO_ESSENCE "エッセンスが足りない。"

#define GAME_MESSAGE_TARGET_SET "ターゲット決定。"
#define GAME_MESSAGE_TARGET_ABORT "ターゲット解除。"

#define GAME_MESSAGE_TARGET_CANCELED "中断した。"
#else

#define GAME_MESSAGE_COMPLETE_QUEST "You completed the quest."
#define GAME_MESSAGE_IMPROVEMENT_FAILED "The improvement failed."
#define GAME_MESSAGE_RESIST_THE_EFFECT "You resist the effects."
#define GAME_MESSAGE_IS_IMMUNE "is immune."
#define GAME_MESSAGE_RESISTED "resists."
#define GAME_MESSAGE_IS_UNAFFECTED "You are unaffected."
#define GAME_MESSAGE_NO_EFFECT "There is no effect."
#define GAME_MESSAGE_IS_CONFUSED "is confused"
#define GAME_MESSAGE_IS_BLIND "is blind!"
#define GAME_MESSAGE_CREATURE_IN_THE_WAY "There is a creature in the way."
#define GAME_MESSAGE_MANA_RECOVERLY "You feel your head clear."
#define GAME_MESSAGE_SUCCESS_PICKING "You have picked the lock."
#define GAME_MESSAGE_FAILED_PICKING "You failed to pick the lock."
#define GAME_MESSAGE_FOUND_SOMETHING "You have found something!"

#define GAME_MESSAGE_BREAK_P_RUNE "The rune of protection is broken!"
#define GAME_MESSAGE_BREAK_E_RUNE "The rune explodes!"
#define GAME_MESSAGE_DISARM_E_RUNE "An explosive rune was disarmed."
#define GAME_MESSAGE_HEAR_NOISE "You hear noise."

#define GAME_MESSAGE_NO_STUDY_SPELL "No spells to browse."
#define GAME_MESSAGE_NO_STUDY_TECH "No techniques to browse."


#define GAME_MESSAGE_BLACK_AURA_TO_OBJECT "A terrible black aura blasts your %s!"
#define GAME_MESSAGE_REMOVED_OBJECT_CURSE "You feel as if someone is watching over you."
#define GAME_MESSAGE_COMBAT_TALK_STAIR "'What's a fucking stair!?'"
#define GAME_MESSAGE_PERILOUS_SECRET "You are too weak to control the stone!"
#define GAME_MESSAGE_RESISTED_FEAR "You stand your ground!"
#define GAME_MESSAGE_NO_MONEY "You do not have the gold."
#define GAME_MESSAGE_NO_MONEY_INSULT "Hey! You don't have the gold - get out of here!"
#define GAME_MESSAGE_BOUNTY_1 "Convert %s into money?"

#define GAME_MESSAGE_TRAP_WARNING "*Leaving trap detect region!*"
#define GAME_MESSAGE_TRAP_DESTROY "You destroyed the trap."

#define GAME_MESSAGE_LITE_FAINT "Your light is growing faint."
#define GAME_MESSAGE_LITE_OUT "Your light has gone out!"

#define GAME_MESSAGE_FAILED_RUNAWAY "You are failed to run away."

#define GAME_MESSAGE_TRAP_NEEDLE "A small needle has pricked you!"
#define GAME_MESSAGE_TRAP_DARTS "A small dart hits you!"
#define GAME_MESSAGE_TRAP_DARTS_MISSED "A small dart barely misses you."
#define GAME_MESSAGE_TRAP_FIRE "You are enveloped in flames."
#define GAME_MESSAGE_TRAP_ACID "You are splashed with acid."
#define GAME_MESSAGE_TRAP_ACID_FLOW "Suddenly, the room is filled with acid."
#define GAME_MESSAGE_TRAP_POISON_FLOW "Suddenly, the room is filled with poisonous liquid."
#define GAME_MESSAGE_TRAP_PIRANHAS "Suddenly, the room is filled with water with piranhas!"

#define GAME_MESSAGE_TRAP_POISONOUS_GAS "A puff of green gas surrounds you!"
#define GAME_MESSAGE_TRAP_PARALYZE_GAS "A puff of yellow gas surrounds you!"
#define GAME_MESSAGE_TRAP_SUMMONING "You are enveloped in a cloud of smoke!"
#define GAME_MESSAGE_TRAP_S_ELEMENTAL "Elemental beings appear to protect their treasures!"
#define GAME_MESSAGE_TRAP_S_BIRD "A storm of birds swirls around you!"
#define GAME_MESSAGE_TRAP_S_H_DEMON "Demons materialize in clouds of fire and brimstone!"
#define GAME_MESSAGE_TRAP_S_H_DRAGON "Draconic forms loom out of the darkness!"
#define GAME_MESSAGE_TRAP_S_HYBRID "Creatures strange and twisted assault you!"
#define GAME_MESSAGE_TRAP_S_VORTEX "Vortices coalesce and wreak destruction!"
#define GAME_MESSAGE_TRAP_E_RUNE "Hideous voices bid:  'Let the darkness have thee!'"
#define GAME_MESSAGE_TRAP_EXPLOSIVE "There is a sudden explosion! Everything inside the chest is destroyed!"
#define GAME_MESSAGE_TRAP_SCATTER  "The contents of the chest scatter all over the dungeon!"
#define GAME_MESSAGE_TRAP_TELEPORT "You hit a teleport trap!"
#define GAME_MESSAGE_TRAP_TY_CURSE "There is a flash of shimmering light!"
#define GAME_MESSAGE_TRAP_ARMAGEDDON "Suddenly, you are surrounded by immotal beings!"
#define GAME_MESSAGE_TRAP_ALARM "An alarm sounds!"

#define GAME_MESSAGE_DAMAGE_DENSITY "Your molecules feel disrupted!"
#define GAME_MESSAGE_RCOK_CRUSHING "You are being crushed!"

#define GAME_MESSAGE_SEEM_TO_CURSE "Hmmm, it seems to be cursed."
#define GAME_MESSAGE_PREVENT_BY_RIDING "You cannot do it when riding."
#define GAME_MESSAGE_PREVENT_BY_CONFUSION "You are too confused."
#define GAME_MESSAGE_PREVENT_BY_BLIND "You are blind!"
#define GAME_MESSAGE_PREVENT_BY_NO_LITE "You have no light to read by."
#define GAME_MESSAGE_PREVENT_BY_HALLUCINATION "You are too hallucinated!"
#define GAME_MESSAGE_PREVENT_BY_STUNED "You are too stuned!"
#define GAME_MESSAGE_PREVENT_BY_PET "You need concentration on the pets now."
#define GAME_MESSAGE_PREVENT_BY_NO_MIRROR "Here are not any mirrors!"
#define GAME_MESSAGE_PREVENT_BY_NO_BAREBAND "You need to be bare hand."
#define GAME_MESSAGE_PREVENT_BY_HP "You do not have enough hit-point to use this power."
#define GAME_MESSAGE_PREVENT_BY_MP "You do not have enough mana to use this power."
#define GAME_MESSAGE_PREVENT_BY_LEVEL "You need to attain level %d to use this power."
#define GAME_MESSAGE_PREVENT_BY_ANTIMAGIC "An anti-magic shell disrupts your magic!"
#define GAME_MESSAGE_PREVENT_BY_TIMESTOP "It shows no reaction."

#define GAME_MESSAGE_ATTEMPT_ANYWAY "Attempt it anyway? "

#define GAME_MESSAGE_IMPROVE_LIMIT "This item is no more able to be improved."

#define GAME_MESSAGE_NO_HAPPEN "Nothing happens."
#define GAME_MESSAGE_CONVERT_FAILED "You failed to convert."
#define GAME_MESSAGE_RIDING_FAILED "You failed to ride."
#define GAME_MESSAGE_NO_DICRECTION_CREATURE "There is no creature."

#define GAME_MESSAGE_FALL_RIDING "You have fallen from your riding pet."

#define GAME_MESSAGE_FIND_TRAP "You have found a trap."
#define GAME_MESSAGE_FIND_DOOR "You have found a secret door."

#define GAME_MESSAGE_TELEPORT_PREVENTED "A mysterious force prevents you from teleporting!"

#define GAME_MESSAGE_PATTERN_LIMIT1 "If you start walking the Pattern, you must walk the whole way. Ok? "
#define GAME_MESSAGE_PATTERN_LIMIT2 "You must start walking the Pattern from the startpoint."
#define GAME_MESSAGE_PATTERN_LIMIT3 "You must walk the Pattern in correct order."
#define GAME_MESSAGE_PATTERN_LIMIT4 "You may not step off from the Pattern."
#define GAME_MESSAGE_PATTERN_GOAL "This section of the Pattern looks less powerful."
#define GAME_MESSAGE_PATTERN_WRECKED "You bleed on the Pattern! Something terrible happens!"

#define GAME_MESSAGE_ARENA_LIMIT "The arena absorbs all attempted magic!"
#define GAME_MESSAGE_ARENA_BERSERK "You cannot think directly!"

#define GAME_MESSAGE_SEE_OBJECT "You see %s."
#define GAME_MESSAGE_GET_MONEY "You collect %ld gold pieces worth of %s."
#define GAME_MESSAGE_PACK_NO_ROOM "You have no room for %s."
#define GAME_MESSAGE_PACK_NO_ROOM_FLOOR "You have no room for any of the objects on the floor."
#define GAME_MESSAGE_PACK_OVERFLOW "Your pack overflows!"
#define GAME_MESSAGE_PACK_REORDER "You reorder some items in your pack."
#define GAME_MESSAGE_PACK_COMBINE "You combine some items in your pack."
#define GAME_MESSAGE_STAFF_UNSTACK "You unstack your staff."

#define GAME_MESSAGE_UMBUSHED "You are ambushed !"

#define GAME_MESSAGE_PROJECTILE_HITS "The %s hits %s."

#define GAME_MESSAGE_FIELD_MAGIC_FAILED1 "The object resists the spell."

#define GAME_MESSAGE_SMITH_USE_ESSENCE "It will take %d essences."
#define GAME_MESSAGE_SMITH_NO_ESSENCE "You don't have enough essences."

#define GAME_MESSAGE_TARGET_SET "Target selected."
#define GAME_MESSAGE_TARGET_ABORT "Target aborted."

#define GAME_MESSAGE_TARGET_CANCELED "Canceled."
#endif

#ifdef JP
#define GAME_MESSAGE_BLOE_AWAY "%sを吹き飛ばした！"
#define GAME_MESSAGE_CAMP_GET_ANGRY "%sは怒った！"
#else
#define GAME_MESSAGE_BLOE_AWAY "You blow %s away!"
#define GAME_MESSAGE_CAMP_GET_ANGRY "%^s gets angry!"
#endif

#ifdef JP
#define GAME_MESSAGE_RESTORE_SOME_MP "少し頭がハッキリした。"
#else
#define GAME_MESSAGE_RESTORE_SOME_MP "You feel your head clear a little."
#endif


// messages of detection

#ifdef JP

#define GAME_MESSAGE_DETECT_TRAPS "トラップの存在を感じとった！"
#define GAME_MESSAGE_DETECT_DOORS "ドアの存在を感じとった！"
#define GAME_MESSAGE_DETECT_STAIRS "階段の存在を感じとった！"
#define GAME_MESSAGE_DETECT_B_TREASURE "埋蔵された財宝の存在を感じとった！"
#define GAME_MESSAGE_DETECT_TREASURE "財宝の存在を感じとった！"
#define GAME_MESSAGE_DETECT_OBJECTS "アイテムの存在を感じとった！"
#define GAME_MESSAGE_DETECT_M_OBJECTS "魔法のアイテムの存在を感じとった！"
#define GAME_MESSAGE_DETECT_CREATURES "クリーチャーの存在を感じとった！"
#define GAME_MESSAGE_DETECT_I_CREATURES "透明な生物の存在を感じとった！"
#define GAME_MESSAGE_DETECT_E_CREATURES "邪悪なる生物の存在を感じとった！"
#define GAME_MESSAGE_DETECT_S_CREATURES "殺気を感じとった！"
#define GAME_MESSAGE_DETECT_SOMEONE "%sの存在を感じとった！"

#else

#define GAME_MESSAGE_DETECT_TRAPS "You sense the presence of traps!"
#define GAME_MESSAGE_DETECT_DOORS "You sense the presence of doors!"
#define GAME_MESSAGE_DETECT_STAIRS "You sense the presence of stairs!"
#define GAME_MESSAGE_DETECT_B_TREASURE "You sense the presence of buried treasure!"
#define GAME_MESSAGE_DETECT_TREASURE "You sense the presence of treasure!"
#define GAME_MESSAGE_DETECT_OBJECTS "You sense the presence of objects!"
#define GAME_MESSAGE_DETECT_M_OBJECTS "You sense the presence of magic objects!"
#define GAME_MESSAGE_DETECT_CREATURES "You sense the presence of creatures!"
#define GAME_MESSAGE_DETECT_I_CREATURES "You sense the presence of invisible creatures!"
#define GAME_MESSAGE_DETECT_E_CREATURES "You sense the presence of evil creatures!"
#define GAME_MESSAGE_DETECT_S_CREATURES "You sense the presence of someone's mind!"
#define GAME_MESSAGE_DETECT_SOMEONE "You sense the presence of %s!"

#endif


#ifdef JP
#define GAME_MESSAGE_FEELING_QUESTDUN "典型的なクエストのダンジョンのようだ。"
#define GAME_MESSAGE_FEELING_WILD_POINT "何かありそうな荒野のようだ。"
#define GAME_MESSAGE_FEELING_TOWN "典型的な町のようだ。"
#define GAME_MESSAGE_FEELING_WILD "典型的な荒野のようだ。"
#else
#define GAME_MESSAGE_FEELING_QUESTDUN "Looks like a typical quest level.";
#define GAME_MESSAGE_FEELING_WILD_POINT "Looks like a strange wilderness."
#define GAME_MESSAGE_FEELING_TOWN "Looks like a typical town."
#define GAME_MESSAGE_FEELING_WILD "Looks like a typical wilderness."
#endif

#ifdef JP
#define GAME_MESSAGE_CANNOT_GO_MORE "それ以上先には進めない。"
#else
#define GAME_MESSAGE_CANNOT_GO_MORE "You cannot go any more."
#endif

#ifdef JP
#define GAME_MESSAGE_BUILDING_NO_BLDG "ここには建物はない。"
#define GAME_MESSAGE_BUILDING_LOSER "「敗者に用はない。」"
#define GAME_MESSAGE_BUILDING_ARENA_FIGHT "ゲートは閉まっている。相手があなたを待っている！"
#define GAME_MESSAGE_BUILDING_NO_RIGHT "それを選択する権利はありません！"

#define GAME_MESSAGE_GET_PRICE "賞金 %ld＄を手に入れた。"

#else
#define GAME_MESSAGE_BUILDING_NO_BLDG "You see no building here."
#define GAME_MESSAGE_BUILDING_LOSER "'There's no place here for a LOSER like you!'"
#define GAME_MESSAGE_BUILDING_ARENA_FIGHT "The gates are closed.  The enemy awaits!"
#define GAME_MESSAGE_BUILDING_NO_RIGHT "You have no right to choose that!"

#define GAME_MESSAGE_GET_PRICE "You get %ldgp."

#endif


// Cause of Death
#if JP

#define COD_QUITTING "途中終了"
#define COD_INTERRUPTING "強制終了"
#define COD_CHEATING_DEATH "死の欺き"
#define COD_PANIC_SAVE "(緊急セーブ)"
#define COD_RETIRE "引退"
#define COD_SEPPUKU "切腹"
#define COD_TRAP "トラップ"
#define COD_POISON "毒"
#define COD_WOUND "致命傷"
#define COD_DART_TRAP "ダーツの罠"
#define COD_PERILOUS_IDENTIFY "危険な秘密"
#define COD_TRIPPING "転倒"
#define COD_FALL_RIDING "落馬"
#define COD_POTION_OF_RUIN "破滅の薬"
#define COD_POISON_NEEDLE "毒針"
#define COD_SCROLL_OF_FIRE "炎の巻物"
#define COD_SCROLL_OF_ICE "氷の巻物"
#define COD_SCROLL_OF_LOGRUS "ログルスの巻物"
#define COD_POISONOUS_FOOD "毒入り食料"
#define COD_CHEST_DP_TRAP "破滅のトラップの宝箱"
#define COD_SUNLIGHT "日光"
#define COD_PATTERN_DAMAGE_1 "「パターン」を歩いたダメージ"
#define COD_PATTERN_DAMAGE_2 "壊れた「パターン」を歩いたダメージ"
#define COD_PATTERN_DAMAGE_3 "パターン損壊"
#define COD_MISCAST_DEATH_SPELL "暗黒魔法の逆流"
#define COD_PIT_TRAP "落とし穴"
#define COD_SPIKED_PIT_TRAP "トゲのある落とし穴"
#define COD_EXPLOADING_CHEST "爆発する箱"
#define COD_POTION_OF_DEATH "死の薬"
#define COD_CALL_OF_VOLD "自殺的な虚無招来"
#define COD_TRAP_DOOR "落とし戸"
#define COD_GENOCIDE_COST "%^sの呪文を唱えた疲労"
#define COD_GENOCIDE_CREATURE "クリーチャー消滅"
#define COD_GENOCIDE_SYMBOL "抹殺"
#define COD_GENOCIDE_MASS "周辺抹殺"
#define COD_GENOCIDE_UNDEAD "アンデッド消滅"
#define COD_HP_TO_MP "HPからMPへの無謀な変換"
#define COD_HP_TO_MP_PASSIVE "頭に昇った血"
#define COD_HARD_CONCENTRATION "過度の集中"
#define COD_ABSTINENCE "禁欲を強いた疲労"
#define COD_BUNP_WALL "壁への衝突"
#define COD_DENSITY "密度"
#define COD_SOLID_ROCK "硬い岩"
#define COD_DROWNING "溺れ"
#define COD_FIRE_AURA "炎のオーラ"
#define COD_COLD_AURA "冷気のオーラ"
#define COD_ELEC_AURA "電撃のオーラ"
#define COD_LETHAL_MUTATION "致命的な突然変異"
#define COD_POLYMORPHED_WOUND "変化した傷"
#else

#define COD_QUITTING "Quitting"
#define COD_INTERRUPTING "Interrupting"
#define COD_CHEATING_DEATH "cheating death"
#define COD_PANIC_SAVE "(panic save)"
#define COD_RETIRE "retired"
#define COD_SEPPUKU "seppuku"
#define COD_TRAP "a trap"
#define COD_POISON "poison"
#define COD_WOUND "a fatal wound"
#define COD_DART_TRAP "a dart trap"
#define COD_PERILOUS_IDENTIFY "perilous secrets"
#define COD_FALL_RIDING "Falling from riding"
#define COD_TRIPPING "Tripping"
#define COD_POTION_OF_RUIN "a potion of ruin"
#define COD_POISON_NEEDLE "a potion needle"
#define COD_SCROLL_OF_FIRE "a Scroll of Fire"
#define COD_SCROLL_OF_ICE "a Scroll of Ice"
#define COD_SCROLL_OF_LOGRUS "a Scroll of Logrus"
#define COD_POISONOUS_FOOD "poisonous food"
#define COD_CHEST_DP_TRAP "a chest dispel-player trap"
#define COD_SUNLIGHT "sunlight"
#define COD_PATTERN_DAMAGE_1 "walking the Pattern"
#define COD_PATTERN_DAMAGE_2 "walking the corrupted Pattern"
#define COD_PATTERN_DAMAGE_3 "corrupting the Pattern"
#define COD_MISCAST_DEATH_SPELL "a miscast Death spell"
#define COD_PIT_TRAP "a pit trap"
#define COD_SPIKED_PIT_TRAP "a spiked pit"
#define COD_EXPLOADING_CHEST "an exploding chest"
#define COD_POTION_OF_DEATH "a potion of Death"
#define COD_CALL_OF_VOLD "a suicidal Call the Void"
#define COD_TRAP_DOOR "a trap door"
#define COD_GENOCIDE_COST "the strain of casting %^s"
#define COD_GENOCIDE_CREATURE "Genocide one"
#define COD_GENOCIDE_SYMBOL "Genocide"
#define COD_GENOCIDE_MASS "Mass Gecocide"
#define COD_GENOCIDE_UNDEAD "Annihilate Undead"
#define COD_HP_TO_MP "Thoughtless convertion from HP to MP"
#define COD_HP_TO_MP_PASSIVE "blood rushing to the head"
#define COD_HARD_CONCENTRATION "Concentrating too hard"
#define COD_ABSTINENCE "the strain of forcing abstinence"
#define COD_BUNP_WALL "bumping into wall"
#define COD_DENSITY "density"
#define COD_SOLID_ROCK "solid rock"
#define COD_DROWNING "drowning"
#define COD_FIRE_AURA "fire aura"
#define COD_COLD_AURA "cold aura"
#define COD_ELEC_AURA "elec aura"
#define COD_LETHAL_MUTATION "a lethal mutation"
#define COD_POLYMORPHED_WOUND "a polymorphed wound"
#endif

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

#ifdef JP
#define TEXT_FILES_GAME_NEWS "news_j.txt"
#define TEXT_FILES_ARMOUR_CURSED_ARTIFACT "a_cursed_j.txt"
#define TEXT_FILES_ARMOUR_LOW_ARTIFACT "a_low_j.txt"
#define TEXT_FILES_ARMOUR_MED_ARTIFACT "a_med_j.txt"
#define TEXT_FILES_ARMOUR_HIGH_ARTIFACT "a_high_j.txt"

#define TEXT_FILES_WEAPON_CURSED_ARTIFACT "w_cursed_j.txt"
#define TEXT_FILES_WEAPON_LOW_ARTIFACT "w_low_j.txt"
#define TEXT_FILES_WEAPON_MED_ARTIFACT "w_med_j.txt"
#define TEXT_FILES_WEAPON_HIGH_ARTIFACT "w_high_j.txt"
#define TEXT_FILES_CHAINSWORD "chainswd_j.txt"

#define TEXT_FILES_ERROR "error_j.txt"
#define TEXT_FILES_DEATH "death_j.txt"
#define TEXT_FILES_SEPPUKU "seppuku_j.txt"
#define TEXT_FILES_CREATURE_FEAR "monfear_j.txt"
#define TEXT_FILES_CREATURE_PET "monpet_j.txt"

#define TEXT_FILES_CREATURE_FRIENDLY "monfrien_j.txt"
#define TEXT_FILES_CREATURE_SPEAK "monspeak_j.txt"
#define TEXT_FILES_RUMOR "rumors_j.txt"
#define TEXT_FILES_SILLY "silly_j.txt"
#define TEXT_FILES_NEWS "news_j.txt"

#define TEXT_FILES_OPTION "joption.txt"
#define TEXT_FILES_OPTION_AUTOSAVE "joption.txt#Autosave"
#define TEXT_FILES_OPTION_WINDOW "joption.txt#Window"
#define TEXT_FILES_OPTION_BASEDELAY "joption.txt#BaseDelay"
#define TEXT_FILES_OPTION_HITPOINT "joption.txt#Hitpoint"

#define TEXT_FILES_OPTION_MANAPOINT "joption.txt#Manapoint"
#define TEXT_FILES_EDITOR "jeditor.txt"
#define TEXT_FILES_BIRTH_AUTO_ROLLER "jbirth.txt#AutoRoller"
#define TEXT_FILES_GAMBLING "jgambling.txt"
#define TEXT_FILES_ARENA "jarena.txt"

#define TEXT_FILES_BUILDING "jbldg.txt"
#define TEXT_FILES_TIME_NORMAL "timenorm_j.txt"
#define TEXT_FILES_TIME_FUN "timefun_j.txt"
#define TEXT_FILES_HELPINFO "jhelpinfo.txt"
#define TEXT_FILES_DEAD "dead_j.txt"

#define TEXT_FILES_ELVISH "elvish.txt"
#define TEXT_FILES_HISTEDIT_N "histedit-%s.prf"
#define TEXT_FILES_HISTEDIT "histedit.prf"

#else

#define TEXT_FILES_GAME_NEWS "news.txt"
#define TEXT_FILES_ARMOUR_CURSED_ARTIFACT "a_cursed.txt"
#define TEXT_FILES_ARMOUR_LOW_ARTIFACT "a_low.txt"
#define TEXT_FILES_ARMOUR_MED_ARTIFACT "a_med.txt"
#define TEXT_FILES_ARMOUR_HIGH_ARTIFACT "a_high.txt"

#define TEXT_FILES_WEAPON_CURSED_ARTIFACT "w_cursed.txt"
#define TEXT_FILES_WEAPON_LOW_ARTIFACT "w_low.txt"
#define TEXT_FILES_WEAPON_MED_ARTIFACT "w_med.txt"
#define TEXT_FILES_WEAPON_HIGH_ARTIFACT "w_high.txt"
#define TEXT_FILES_CHAINSWORD "chainswd.txt"

#define TEXT_FILES_ERROR "error.txt"
#define TEXT_FILES_DEATH "death.txt"
#define TEXT_FILES_SEPPUKU "seppuku.txt"
#define TEXT_FILES_CREATURE_FEAR "monfear.txt"
#define TEXT_FILES_CREATURE_PET "monpet.txt"

#define TEXT_FILES_CREATURE_FRIENDLY "monfrien.txt"
#define TEXT_FILES_CREATURE_SPEAK "monspeak.txt"
#define TEXT_FILES_RUMOR "rumors.txt"
#define TEXT_FILES_SILLY "silly.txt"
#define TEXT_FILES_NEWS "news.txt"

#define TEXT_FILES_OPTION "option.txt"
#define TEXT_FILES_OPTION_AUTOSAVE "option.txt#Autosave"
#define TEXT_FILES_OPTION_WINDOW "option.txt#Window"
#define TEXT_FILES_OPTION_BASEDELAY "option.txt#BaseDelay"
#define TEXT_FILES_OPTION_HITPOINT "option.txt#Hitpoint"

#define TEXT_FILES_OPTION_MANAPOINT "option.txt#Manapoint"
#define TEXT_FILES_EDITOR "editor.txt"
#define TEXT_FILES_BIRTH_AUTO_ROLLER "birth.txt#AutoRoller"
#define TEXT_FILES_GAMBLING "gambling.txt"
#define TEXT_FILES_ARENA "arena.txt"

#define TEXT_FILES_BUILDING "bldg.txt"
#define TEXT_FILES_TIME_NORMAL "timenorm.txt"
#define TEXT_FILES_TIME_FUN "timefun.txt"
#define TEXT_FILES_HELPINFO "helpinfo.txt"
#define TEXT_FILES_DEAD "dead.txt"

#define TEXT_FILES_ELVISH "elvish.txt"
#define TEXT_FILES_HISTEDIT_N "histpref-%s.prf"
#define TEXT_FILES_HISTEDIT "histpref.prf"

#endif


static const cptr effect_message_in_blind[MAX_DO_EFFECT] =
{
#if JP
	"何かに攻撃された。",
	"電撃で攻撃された。",
	"毒で攻撃された。",
	"酸で攻撃された。",
	"冷気で攻撃された。",

	"火炎で攻撃された。",
	"エネルギーの塊で攻撃された。",
	"何かで攻撃された。",
	"何か鋭いもので攻撃された。",
	"何かとても熱いもので攻撃された。",
//10
	"何か湿ったもので攻撃された。",
	"何かで攻撃された。",
	"何かで攻撃された。",
	"何かで攻撃された。",
	"",

	"何か鋭いもので攻撃された。",
	"轟音で攻撃された。",
	"何か混乱するもので攻撃された。",
	"運動エネルギーで攻撃された。",
	"何か遅いもので攻撃された。",
//20
	"",
	"何かが空からあなたの頭上に落ちてきた。",
	"何か鋭く冷たいもので攻撃された。",
	"無秩序の波動で攻撃された。",
	"地獄の力で攻撃された。",

	"何かさえないもので攻撃された。",
	"何か奇妙なもので攻撃された。",
	"過去からの衝撃に攻撃された！",
	"何か重いもので攻撃された！",
	"",
//30
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"何らかの攻撃によって気分がよくなった。",
	"何かで攻撃された！",
//40
	"何か遅いもので攻撃された！",
	"",
	"眠ってしまった！",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//50
	"",
	"",
	"",
	"",
	"",

	"",
	"放射能で攻撃された！",
	"爆発があった！",
	"",
	"",
//60
	"何か非常に冷たいもので攻撃された！",
	"何かで攻撃された！",
	"何かで攻撃された！",
	"純粋なエネルギーで攻撃された！",
	"",

	"",
	"",
	"",
	"",
	"",
//70
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//80
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//90
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//100
	"",
	"",
	"",

#else
	"You are hit by something.",
	"You are hit by lightning.",
	"You are hit by poison.",
	"You are hit by acid.",
	"You are hit by cold.",

	"You are hit by fire.",
	"You are hit by an energy.",
	"You are hit by something.",
	"You are hit by something sharp.",
	"You are hit by something *HOT*.",
//10
	"You are hit by something wet.",
	"You are hit by something.",
	"You are hit by something.",
	"You are hit by something.",
	"",

	"You are hit by something sharp.",
	"You are hit by a loud noise.",
	"You are hit by something puzzling.",
	"You are hit by kinetic force.",
	"You are hit by something slow.",
//20
	"",
	"Something falls from the sky on you.",
	"You are hit by something sharp and cold.",
	"You are hit by a wave of anarchy."
	"You are hit by nether forces.",

	"You are hit by something.",
	"You are hit by something strange.",
	"You are hit by a blast from the past!",
	"You are hit by something heavy!",
	"",
//30
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"You are hit by something invigorating!",
	"You are hit by something!",
//40
	"You are hit by something slow!",
	"",
	"You fall asleep!",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//50
	"",
	"",
	"",
	"",
	"",

	"",
	"You are hit by radiation!",
	"There is an explosion!",
	"",
	"",
//60
	"You are hit by something extremely cold!",
	"You are hit by something!",
	"You are hit by something!",
	"You are hit by pure energy!",
	"",

	"",
	"",
	"",
	"",
	"",
//70
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//80
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//90
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//100
	"",
	"",
	"",

#endif
};

#ifdef JP
	#define PROMPT_COMMAND "コマンド: "
	#define PROMPT_KEYPRESS "押すキー: "
	#define PROMPT_FILE "ファイル: "
	#define PROMPT_MEMO "メモ: "
	#define PROMPT_TRIGGER_KEY "トリガーキー: "
	#define PROMPT_INVENTORY "持ち物: "
	#define PROMPT_EQUIPMENT "装備品: "
	#define PROMPT_COUNT "回数: "
	#define PROMPT_REST "休憩 (0-9999, '*' で HP/MP全快, '&' で必要なだけ): "
	#define PROMPT_PICK "%sを拾いますか? " 
	#define PROMPT_DIRECTION "方向 (ESCで中断)? "
#else
	#define PROMPT_COMMAND "Command: "
	#define PROMPT_KEYPRESS "Keypress: "
	#define PROMPT_FILE "File: "
	#define PROMPT_MEMO "Memo: "
	#define PROMPT_TRIGGER_KEY "Trigger: "
	#define PROMPT_INVENTORY "Inven: "
	#define PROMPT_EQUIPMENT "Equip: "
	#define PROMPT_COUNT "Count: "
	#define PROMPT_REST "Rest (0-9999, '*' for HP/SP, '&' as needed): "
	#define PROMPT_PICK "Pick up %s? " 
	#define PROMPT_DIRECTION "Direction (Escape to cancel)? "
#endif

#ifdef JP
#define MES_GET_CHECK_AHEAD "本当にこのまま進むか？"
#define MES_GET_CHECK_SUICIDE "本当に自殺しますか？"
#define MES_GET_CHECK_LEVEL_TELEPORT "本当に他の階にテレポートしますか？"
#else
#define MES_GET_CHECK_AHEAD "Really want to go ahead? "
#define MES_GET_CHECK_SUICIDE "Do you really want to commit suicide? "
#define MES_GET_CHECK_LEVEL_TELEPORT "Are you sure? (Teleport Level)"
#endif

#ifdef JP
	#define ST_VIEW_HUNGER_WEAK		"衰弱  "
	#define ST_VIEW_HUNGER_HUNGRY	"空腹  "
	#define ST_VIEW_HUNGER_NORMAL	"      "
	#define ST_VIEW_HUNGER_FULL		"満腹  "
	#define ST_VIEW_HUNGER_GORGED	"食過ぎ"
#else
	#define ST_VIEW_HUNGER_WEAK		"Weak  "
	#define ST_VIEW_HUNGER_HUNGRY	"Hungry"
	#define ST_VIEW_HUNGER_NORMAL	"      "
	#define ST_VIEW_HUNGER_FULL		"Full  "
	#define ST_VIEW_HUNGER_Gorged	"Gorges"
#endif


#ifdef JP
	#define SKILL_NAME_MAGIC "魔法"
	#define SKILL_NAME_PSIONIC "超能力"
	#define SKILL_NAME_IMITATION "ものまね"
	#define SKILL_NAME_HISSATSU "必殺剣"
	#define SKILL_NAME_MIRROR_M "鏡魔法"
	#define SKILL_NAME_NINJUTSU "忍術"
	#define SKILL_NAME_PRAYER "祈り"
#else
	#define SKILL_NAME_MAGIC "magic"
	#define SKILL_NAME_PSIONIC "psionic powers"
	#define SKILL_NAME_IMITATION "imitation"
	#define SKILL_NAME_HISSATSU "hissatsu"
	#define SKILL_NAME_MIRROR_M "mirror magic"
	#define SKILL_NAME_NINJUTSU "ninjutsu"
	#define SKILL_NAME_PRAYER "prayer"
#endif

#ifdef JP

#define DIARY_CREATE_PLAYER "-------- プレイヤー作成 --------"
#define DIARY_GAMEOVER "-------- ゲームオーバー --------"
#define DIARY_SAVEQUIT "----ゲーム中断----"
#define DIARY_GAMERESTART "----ゲーム再開----"
#define DIARY_GAMEABORT "----ゲーム異常終了----"
#define DIARY_RETIRED "ダンジョンの探索から引退した。"
#define DIARY_SUICIDE "絶望して自殺した。"
#define DIARY_WIZARD "ウィザードモードに突入してスコアを残せなくなった。"
#define DIARY_UP_STAIR "階段を上った"
#define DIARY_DOWN_STAIR "階段を下りた"

#else

#define DIARY_CREATE_PLAYER "-------- Create Player --------"
#define DIARY_GAMEOVER "--------   Game  Over   --------"
#define DIARY_SAVEQUIT "---- Save and Exit Game ----"
#define DIARY_GAMERESTART "---- Restart Game ----"
#define DIARY_GAMEABORT "---- Panic Save and Abort Game ----"
#define DIARY_RETIRED "retired exploring dungeons."
#define DIARY_SUICIDE "give up all hope to commit suicide."
#define DIARY_WIZARD "give up sending score to use wizard mode."
#define DIARY_UP_STAIR 1"climbed up the stairs to"
#define DIARY_DOWN_STAIR "climbed down the stairs to"
#endif

#ifdef JP
#define SHUFFLE_START "あなたはカードを切って一枚引いた..."
#define SHUFFLE_TELEPORT "テレポート・カードだ。"
#define SHUFFLE_FRIEND "友好的なクリーチャーの絵だ。"
#define SHUFFLE_DEATH "なんてこった！《死》だ！"
#define SHUFFLE_DEVIL "なんてこった！《悪魔》だ！"
#define SHUFFLE_HANGEDMAN "なんてこった！《吊られた男》だ！"
#define SHUFFLE_DISCODE "《不調和の剣》だ。"
#define SHUFFLE_FOOL "《愚者》だ。"
#define SHUFFLE_STRANGE "奇妙なクリーチャーの絵だ。"
#define SHUFFLE_MOON "《月》だ。"
#define SHUFFLE_WHEEL "《運命の輪》だ。"
#define SHUFFLE_JUSTICE "《正義》だ。"
#define SHUFFLE_TOWER "《塔》だ。"
#define SHUFFLE_TEMPERANCE "《節制》だ。"
#define SHUFFLE_LOVER "《恋人》だ。"
#define SHUFFLE_HERMIT "《隠者》だ。"
#define SHUFFLE_JUDGEMENT "《審判》だ。"
#define SHUFFLE_SUN "《太陽》だ。"
#define SHUFFLE_WORLD "《世界》だ。"
#else
#define SHUFFLE_START "You shuffle the deck and draw a card..."
#define SHUFFLE_TELEPORT "It's a teleport trump card."
#define SHUFFLE_FRIEND "It's the picture of a friendly creature."
#define SHUFFLE_DEATH "Oh no! It's Death!"
#define SHUFFLE_DEVIL "Oh no! It's the Devil!"
#define SHUFFLE_HANGEDMAN "Oh no! It's the Hanged Man."
#define SHUFFLE_DISCODE "It's the swords of discord."
#define SHUFFLE_FOOL "It's the Fool."
#define SHUFFLE_STRANGE "It's the picture of a strange creature."
#define SHUFFLE_MOON "It's the Moon."
#define SHUFFLE_WHEEL "It's the Wheel of Fortune."
#define SHUFFLE_JUSTICE "It's Justice."
#define SHUFFLE_TOWER "It's the Tower."
#define SHUFFLE_TEMPERANCE "It's Temperance."
#define SHUFFLE_LOVER "It's the Lovers."
#define SHUFFLE_HERMIT "It's the Hermit."
#define SHUFFLE_JUDGEMENT "It's the Judgement."
#define SHUFFLE_SUN "It's the Sun."
#define SHUFFLE_WORLD "It's the World."
#endif

#ifdef JP
#define INVOKE_SPIRIT_START "あなたは死者たちの力を招集した..."
#define INVOKE_SPIRIT_HIGH_POWER "あなたはおどろおどろしい力のうねりを感じた！"
#define INVOKE_SPIRIT_LOW1 "なんてこった！あなたの周りの地面から朽ちた人影が立ち上がってきた！"
#define INVOKE_SPIRIT_LOW2 "名状し難い邪悪な存在があなたの心を通り過ぎて行った..."
#define INVOKE_SPIRIT_LOW3 "あなたの頭に大量の幽霊たちの騒々しい声が押し寄せてきた..."
#define INVOKE_SPIRIT_FUNBLE_AFTER "陰欝な声がクスクス笑う。「もうすぐおまえは我々の仲間になるだろう。弱き者よ。」"
#else
#define INVOKE_SPIRIT_START "You call on the power of the dead..."
#define INVOKE_SPIRIT_HIGH_POWER "You feel a surge of eldritch force!"
#define INVOKE_SPIRIT_LOW1 "Oh no! Mouldering forms rise from the earth around you!"
#define INVOKE_SPIRIT_LOW2 "An unnamable evil brushes against your mind..."
#define INVOKE_SPIRIT_LOW3 "Your head is invaded by a horde of gibbering spectral voices..."
#define INVOKE_SPIRIT_FUNBLE_AFTER "Sepulchral voices chuckle. 'Soon you will join us, mortal.'"
#endif

#ifdef JP
#define DEBUG_MESSAGE_ROOM_NUM "部屋数: %d"
#define DEBUG_MESSAGE_GLASS_ROOM "[ガラスの部屋]"
#define DEBUG_MESSAGE_ROOM_OF "[%sの部屋]"
#define DEBUG_MESSAGE_CRYPT_ROOM "[聖堂]"
#define DEBUG_MESSAGE_TRAP_PIT_ROOM "[%s%sの罠ピット]"
#else
#define DEBUG_MESSAGE_ROOM_NUM "Number of Rooms: %d"
#define DEBUG_MESSAGE_GLASS_ROOM "[Glass Room]"
#define DEBUG_MESSAGE_ROOM_OF "[Room of %s]"
#define DEBUG_MESSAGE_CRYPT_ROOM "[Crypt]"
#define DEBUG_MESSAGE_TRAP_PIT_ROOM "[Trapped creature pit (%s%s)]"
#endif

#ifdef JP
#define EFFECT_MES_MIRROR_CRUSH "鏡が割れた！"
#define EFFECT_MES_MIND_BACKFIRE "%^sの堕落した精神は攻撃を跳ね返した！"
#define EFFECT_MES_DISARM_TRAP "カチッと音がした。"
#else
#define EFFECT_MES_MIRROR_CRUSH "The mirror was crashed!"
#define EFFECT_MES_MIND_BACKFIRE "%^s corrupted mind backlashes your attack!"
#define EFFECT_MES_DISARM_TRAP "Click!"
#endif
