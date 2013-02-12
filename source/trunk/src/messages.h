
// Keywords

#ifdef JP
#define KEYWORD_CURSED "呪われている"
#else
#define KEYWORD_CURSED "cursed"
#endif


#ifdef JP

#define MES_SYS_MORE "-- 続く --"
#define MES_SYS_WIZARD_MODE_ON "ウィザードモード突入。"
#define MES_SYS_WIZARD_MODE_OFF "ウィザードモード解除。"
#define MES_SYS_STARTING_GAME "ゲームを開始します。お待ち下さい..."
#define MES_SYS_FAILED_TEMPFILE "一時ファイル %s を作成できませんでした。"
#define MES_SYS_FAILED_FILEOPEN "ファイルを開けません。"
#define MES_SYS_FAILED_FILEOPEN2 "%s を開くことができませんでした。"
#define MES_SYS_REPLACE_FILE "現存するファイルに上書きしますか? (%s)"
#define MES_SYS_HIT_ANY_KEY "何かキーを押して下さい。"
#define MES_SYS_PLACE_SPACE "場所を選んでスペースキーを押して下さい。"
#define MES_SYS_OPTION_AUX "初期オプション((*)はスコアに影響)"
#define MES_SYS_OPTION_AUX2 "初期オプション(参照のみ)"
#define MES_SYS_AUTO_SAVING "自動セーブ中"
#define MES_SYS_GAME_SAVING "ゲームをセーブしています..."
#define MES_SYS_QUITING_KEY "リターンキーか ESC キーを押して下さい。"
#define MES_SYS_SAVE_FAILED "セーブ失敗！"
#define MES_SYS_SCORE_FAILED "スコア・ファイルが使用できません。"
#define MES_SYS_PANIC_SAVE_SUCCEED "緊急セーブ成功！"
#define MES_SYS_PANIC_SAVE_FAILED "緊急セーブ失敗！"
#define MES_SYS_YOUR_TRAITS "あなたには以下の特性がある"
#define MES_SYS_YOUR_NO_TRAIT "あなたには特別な特性がない"
#define MES_SYS_SERVANTS "%sには%d種類、%d体の従僕がいる"
#define MES_SYS_NO_SERVANT "%sには従僕がいない"
#define MES_SYS_UNKNOWN_SYMBOL "無効な文字"
#define MES_SYS_INVALID_NUM "値がおかしいです。"
#define MES_SYS_ERROR "エラー"
#define MES_SYS_PANIC "緊急事態"
#define MES_SYS_INTERUPTING "強制終了中"
#define MES_SYS_CONNECTING "接続中..."
#define MES_SYS_SENDING_SCORE "スコア送信中..."

#define MES_SYS_OUT_OF_SWITCH "switch構文範囲外"

#define MES_SYS_TODAY "今日"
#define MES_SYS_NORTH "北"
#define MES_SYS_SOUTH "南"
#define MES_SYS_EAST "東"
#define MES_SYS_WEST "西"
#define MES_SYS_MONEY "所持金"
#define MES_SYS_WEIGHT "重量"

#define MES_SYS_MODIFICATION "能力修正"

#define MES_SYS_VISUAL_COMMAND "コマンド (n/N/^N/a/A/^A/c/C/^C/v/V/^V): "
#define MES_SYS_ASK_USE "%sを使いますか？"
#define MES_SYS_ASK_SURE "よろしいですか？"

#define MES_SYS_CHARGE_NUM(NUM) "回分"

#define MES_SYS_SKILL "技能値"
#else

#define MES_SYS_MORE "-- MORE --"
#define MES_SYS_WIZARD_MODE_ON "Wizard mode on."
#define MES_SYS_WIZARD_MODE_OFF "Wizard mode off."
#define MES_SYS_STARTING_GAME "Starting Game, Please wait..."
#define MES_SYS_FAILED_TEMPFILE "Failed to create temporary file %s."
#define MES_SYS_FAILED_FILEOPEN "Can not open file."
#define MES_SYS_FAILED_FILEOPEN2 "Failed to open %s."
#define MES_SYS_REPLACE_FILE "Replace existing file %s? "
#define MES_SYS_HIT_ANY_KEY "Hit Any Key。"
#define MES_SYS_PLACE_SPACE "Select a point and press space."

#define MES_SYS_OPTION_AUX "Birth Option((*)s effect score)"
#define MES_SYS_OPTION_AUX2 "Birth Options(browse only)"
#define MES_SYS_AUTO_SAVING "Autosaving the game..."
#define MES_SYS_GAME_SAVING "Saving game..."
#define MES_SYS_QUITING_KEY "Press Return (or Escape)."
#define MES_SYS_SAVE_FAILED "death save failed!"
#define MES_SYS_SCORE_FAILED "Score file unavailable."
#define MES_SYS_PANIC_SAVE_SUCCEED "Panic save succeeded!"
#define MES_SYS_PANIC_SAVE_FAILED "panic save failed!"
#define MES_SYS_YOUR_TRAITS "You have these trait..."
#define MES_SYS_YOUR_NO_TRAIT "You have no trait"
#define MES_SYS_YOUR_SERVANT "%s have no servant"
#define MES_SYS_YOUR_NO_SERVANT "%s have %d species, %d servants"

#define MES_SYS_UNKNOWN_SYMBOL "unknown symbol"
#define MES_SYS_INVALID_NUM "Invalid response."
#define MES_SYS_ERROR "Error"
#define MES_SYS_PANIC "Panic"
#define MES_SYS_INTERUPTING "Interrupting"

#define MES_SYS_CONNECTING "Connecting..."
#define MES_SYS_SENDING_SCORE "Sending the score..."

#define MES_SYS_OUT_OF_SWITCH "Out of switch."
#define MES_SYS_TODAY "Today"
#define MES_SYS_NORTH " North"
#define MES_SYS_SOUTH " South"
#define MES_SYS_EAST " East"
#define MES_SYS_WEST " West"
#define MES_SYS_MONEY "AU"
#define MES_SYS_WEIGHT "Weight"

#define MES_SYS_MODIFICATION "Modification"

#define MES_SYS_VISUAL_COMMAND "Command (n/N/^N/a/A/^A/c/C/^C/v/V/^V):"
#define MES_SYS_ASK_USE "use %s?"
#define MES_SYS_ASK_SURE "Are you sure?"

#define MES_SYS_CHARGE_NUM(NUM) ((NUM) > 1 ? " charges" : "charge")

#define MES_SYS_SKILL "Proficiency"
#endif
 
#if JP
#define KW_CREATURE "クリーチャー"
#define KW_NAME "名前"
#define KW_RACE "種族"
#define KW_CLASS "職業"
#define KW_CHARA "性格"
#define KW_PATLON "主神"
#define KW_LEVEL "レベル"
#define KW_LEVEL_DEC "レベルx"
#define KW_HP "HP"
#define KW_MP "MP"
#define KW_AC "AC"
#define KW_EV "EV"
#define KW_VO "VO"
#define KW_EXP "ソウル"
#define KW_EXP_DEC "ソウルx"
#define KW_EXP_NEXT "次レベル"
#define KW_EXP_DEC "ソウルx"
#define KW_MONEY "所持金"
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
#define KW_DONE "完了"
#define KW_FAILED "失敗"
#define KW_NONE "なし"
#define KW_SURFACE "地上"
#define KW_QUEST "クエスト"
#define KW_SCORE_RANKING "勇者の殿堂"
#define KW_WINNER "偉大なる者"
#define KW_MAX "最高"
#define KW_RANDOM "ランダム"
#define KW_BACK_TO_START "最初に戻る"
#define KW_QUIT_GAME "終了する"
#define KW_ACCEPT "決定する"
#define KW_IT "何か"
#define KW_SOMETHING "何か"
#define KW_CLONE "クローン"
#define KW_VALIABLE_RACE "種族可変"
#define KW_VALIABLE_CLASS "種族可変"
#define KW_VALIABLE_CHARA "性格可変"
#define KW_MALE "男性"
#define KW_FEMALE "女性"
#define KW_INTERSEX "両性"
#define KW_NOSEX "無性"
#define KW_FAST "加速"
#define KW_SLOW "減速"
#define KW_RIDING "乗馬"
#define KW_ASKING_PRICE "提示価格"
#define KW_FINAL_OFFER_PRICE "最終提示価格"
#define KW_FLOOR_NUM(NUM) "%d階", (NUM)
#define KW_FLOOR_NUM2(DUNGEON, NUM) "%d階(%s)", (NUM), (DUNGEON)
#define KW_DEAD "死亡"
#define KW_ALIVE "生存"
#define KW_UNIDENTIFIED "未鑑定"
#define KW_EMPTY "空"
#define KW_TRIED "未判明"
#define KW_PRICE_OFF "%引き"
#else
#define KW_CREATURE "Creatures"
#define KW_NAME "Name"
#define KW_RACE "Race"
#define KW_CLASS "Class"
#define KW_CHARA "Chara"
#define KW_PATLON "Patron"
#define KW_LEVEL "LEVEL"
#define KW_LEVEL_DEC "Level"
#define KW_HP "HP"
#define KW_MP "MP"
#define KW_AC "AC"
#define KW_EV "EV"
#define KW_VO "VO"
#define KW_EXP "SOUL"
#define KW_EXP_DEC "Soul"
#define KW_EXP_NEXT "NEXT"
#define KW_MONEY "Money"
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
#define KW_DONE "Done"
#define KW_FAILED "Failed"
#define KW_NONE "None"
#define KW_SURFACE "the surface"
#define KW_QUEST "quest"
#define KW_SCORE_RANKING "Hall of Fame"
#define KW_WINNER "Magnificent"
#define KW_MAX "Max"
#define KW_RANDOM "Random"
#define KW_BACK_TO_START "Back to start"
#define KW_QUIT_GAME "Quit game"
#define KW_ACCEPT "Accept"
#define KW_IT "It"
#define KW_SOMETHING "Something"
#define KW_CLONE "Clone"
#define KW_VALIABLE_RACE "VariableRace"
#define KW_VALIABLE_CLASS "VariableClass"
#define KW_VALIABLE_CHARA "VariableChara"
#define KW_MALE "Male"
#define KW_FEMALE "Female"
#define KW_INTERSEX "Intersex"
#define KW_NOSEX "Nosex"
#define KW_FAST "Fast"
#define KW_SLOW "Slow"
#define KW_RIDING "Riding"
#define KW_ASKING_PRICE "Asking Price"
#define KW_FINAL_OFFER_PRICE "Final Offer"
#define KW_FLOOR_NUM(NUM) "Floor %d", (NUM)
#define KW_FLOOR_NUM2(DUNGEON, NUM) "%s L%d:", (DUNGEON), (NUM) 
#define KW_DEAD "dead"
#define KW_ALIVE "alive"
#define KW_UNIDENTIFIED "unidentified"
#define KW_EMPTY "empty"
#define KW_TRIED "tried"
#define KW_PRICE_OFF "%off"
#endif

#ifdef JP
#define MES_FEATURE_KILLED "まばゆい閃光が走った！"
#else
#define MES_FEATURE_KILLED "There is a bright flash of light!"
#endif

#ifdef JP

#define GAME_MESSAGE_TRAP_BLIND "黒いガスに包み込まれた！"
#define GAME_MESSAGE_TRAP_CONFUSE "きらめくガスに包み込まれた！"
#define GAME_MESSAGE_TRAP_POISON "刺激的な緑色のガスに包み込まれた！"
#define GAME_MESSAGE_TRAP_SLEPT "奇妙な白い霧に包まれた！"

#define GAME_MESSAGE_TRAP_NEEDLE "仕掛けられていた小さな針に刺されてしまった！"
#define GAME_MESSAGE_TRAP_DARTS "小さなダーツが飛んできて刺さった！"
#define GAME_MESSAGE_TRAP_DARTS_MISSED "小さなダーツが飛んできた！が、運良く当たらなかった。"
#define GAME_MESSAGE_TRAP_FIRE "炎に包まれた！"
#define GAME_MESSAGE_TRAP_ACID "酸が吹きかけられた！"
#define GAME_MESSAGE_TRAP_ACID_FLOW "突然床から酸が溢れ出した！"
#define GAME_MESSAGE_TRAP_POISON_FLOW "突然床から毒液溢れ出した！"
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

#else
#define GAME_MESSAGE_TRAP_BLIND "A black gas surrounds you!"
#define GAME_MESSAGE_TRAP_CONFUSE "A gas of scintillating colors surrounds you!"
#define GAME_MESSAGE_TRAP_POISON "A pungent green gas surrounds you!"
#define GAME_MESSAGE_TRAP_SLEPT "A strange white mist surrounds you!"

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
#endif

#if JP
#define GAME_MESSAGE_COMPLETE_QUEST "クエストを達成した。"
#define GAME_MESSAGE_CURRENT_QUEST "《遂行中のクエスト》"
#define GAME_MESSAGE_COMPLETED_QUEST "《達成したクエスト》"
#define GAME_MESSAGE_FAILED_QUEST "《失敗したクエスト》"
#define GAME_MESSAGE_REMAINING_QUEST "《残っているクエスト》"

#define GAME_MESSAGE_FAILED "失敗した。"
#define GAME_MESSAGE_ENCHANTMENT_FAILED "強化に失敗した。"
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

#define GAME_MESSAGE_LITE_AREA "光が辺りを覆った。"
#define GAME_MESSAGE_UNLITE_AREA "暗闇が辺りを覆った。"
#define GAME_MESSAGE_LITE_FAINT "明かりが微かになってきている。"
#define GAME_MESSAGE_LITE_OUT "明かりが消えてしまった！"
#define GAME_MESSAGE_LITE_NONE "光源を装備していない。"
#define GAME_MESSAGE_LITE_NOREFILL "この光源は寿命を延ばせない。"

#define GAME_MESSAGE_DESTROY "燃えるような閃光が発生した！"

#define GAME_MESSAGE_FAILED_RUNAWAY "うまく逃げられなかった。"

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
#define GAME_MESSAGE_PREVENT_BY_HUNGER "その前に食事をとらないと。"
#define GAME_MESSAGE_PREVENT_BY_POSTURE "今は構えに集中している。"
#define GAME_MESSAGE_PREVENT_BY_CREATURE(NAME) "%sが邪魔だ。", (NAME)
#define GAME_MESSAGE_FISHING_PREVENT_BY_FEATURE "そこは水辺ではない。"
#define GAME_MESSAGE_MAGIC_PREVENT_BY_DUNGEON "ダンジョンが魔法を吸収した！"
#define GAME_MESSAGE_LITE_PREVENT_BY_DUNGEON "ダンジョンが光を吸収した。"
#define GAME_MESSAGE_TELE_PREVENT_BY_CREATURE "テレポートを邪魔された！"
#define GAME_MESSAGE_HAYAGAKE_PREVENT "ここでは素早く動けない。"
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

#define GAME_MESSAGE_ARENA_READY "試合開始！"
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

#define GAME_MESSAGE_CALL_ARTIFACT "このアーティファクトを何と名付けますか？"
#define GAME_MESSAGE_SMITH_USE_ESSENCE "エッセンスを%d個使用します。"
#define GAME_MESSAGE_SMITH_NO_ESSENCE "エッセンスが足りない。"

#define GAME_MESSAGE_TARGET_SET "ターゲット決定。"
#define GAME_MESSAGE_TARGET_ABORT "ターゲット解除。"
#define GAME_MESSAGE_TARGET_CANCELED "中断した。"

#define GAME_MESSAGE_GOODBYE(NAME) ("さようなら、%s!", (NAME))
#else

#define GAME_MESSAGE_COMPLETE_QUEST "You completed the quest."
#define GAME_MESSAGE_CURRENT_QUEST "< Current Quest >"
#define GAME_MESSAGE_COMPLETED_QUEST "< Completed Quest >"
#define GAME_MESSAGE_FAILED_QUEST "< Failed Quest >"
#define GAME_MESSAGE_REMAINING_QUEST "< Remaining Quest >"

#define GAME_MESSAGE_FAILED "Failed."
#define GAME_MESSAGE_ENCHANTMENT_FAILED "The enchantment failed."
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

#define GAME_MESSAGE_LITE_AREA "You are surrounded by a light."
#define GAME_MESSAGE_UNLITE_AREA "Darkness surrounds you."
#define GAME_MESSAGE_LITE_FAINT "Your light is growing faint."
#define GAME_MESSAGE_LITE_OUT "Your light has gone out!"
#define GAME_MESSAGE_LITE_NONE "You are not wielding a light."
#define GAME_MESSAGE_LITE_NOREFILL "Your light cannot be refilled."

#define GAME_MESSAGE_DESTROY "There is a searing blast of light!"

#define GAME_MESSAGE_FAILED_RUNAWAY "You are failed to run away."

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
#define GAME_MESSAGE_PREVENT_BY_HUNGER "You must eat something here."
#define GAME_MESSAGE_PREVENT_BY_POSTURE "You need concentration on your form."
#define GAME_MESSAGE_PREVENT_BY_CREATURE(NAME) "%^s is stand in your way.", (NAME)
#define GAME_MESSAGE_FISHING_PREVENT_BY_FEATURE "There is no fishing place."
#define GAME_MESSAGE_MAGIC_PREVENT_BY_DUNGEON "The dungeon absorbs all attempted magic!"
#define GAME_MESSAGE_LITE_PREVENT_BY_DUNGEON "The darkness of this dungeon absorb your light."
#define GAME_MESSAGE_TELE_PREVENT_BY_CREATURE "Your teleportation is blocked!"
#define GAME_MESSAGE_HAYAGAKE_PREVENT "You cannot run in here."
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

#define GAME_MESSAGE_ARENA_READY "Ready..Fight!"
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

#define GAME_MESSAGE_CALL_ARTIFACT "What do you want to call the artifact?"

#define GAME_MESSAGE_SMITH_USE_ESSENCE "It will take %d essences."
#define GAME_MESSAGE_SMITH_NO_ESSENCE "You don't have enough essences."

#define GAME_MESSAGE_TARGET_SET "Target selected."
#define GAME_MESSAGE_TARGET_ABORT "Target aborted."
#define GAME_MESSAGE_TARGET_CANCELED "Canceled."

#define GAME_MESSAGE_GOODBYE(NAME) ("Goodbye, %s!", (NAME))
#endif

#ifdef JP
#define MES_PROBING_START "調査中..."
#define MES_PROBING_FORMAT "%s 属性:%s 性別:%s HP:%d/%d AC:%d 速度:%s%d 腕:%d 知:%d 賢:%d 器:%d 耐:%d 魅:%d 経験:"
#else
#define MES_PROBING_START "Probing..."
#define MES_PROBING_FORMAT "%s align:%s sex:%s HP:%d/%d AC:%d speed:%s%d STR:%d INT:%d WIS:%d DEX:%d CON:%d CHA:%d exp:"
#endif

#ifdef JP
#define GAME_MESSAGE_FATAL_SPOT "%sの急所に突き刺さった！"
#define GAME_MESSAGE_SUICIDE_BOMBER "爆発して粉々になった。"
#define GAME_MESSAGE_CARSE_OF_BLOOD_DEAD "%^sは死の間際に恐ろしい血の呪いを%^sにかけた！"
#define GAME_MESSAGE_LOST_LIFE1 "生命力を少し吸い取られた気がする！"
#define GAME_MESSAGE_LOST_LIFE2 "生命力が体から吸い取られた気がする！"
#define GAME_MESSAGE_BLOE_AWAY "%sを吹き飛ばした！"
#define GAME_MESSAGE_CAMP_GET_ANGRY "%sは怒った！"
#define GAME_MESSAGE_HOSTILE "%^sは敵にまわった！"
#define GAME_MESSAGE_BOUNTY_DEAD "%sの首には報奨がかかっている。"
#else
#define GAME_MESSAGE_FATAL_SPOT "Your shot sticked on a fatal spot of %s!"
#define GAME_MESSAGE_SUICIDE_BOMBER "%^s explodes into tiny shreds."
#define GAME_MESSAGE_CARSE_OF_BLOOD_DEAD "On death and dying, %^s puts a terrible blood curse on %^s!"
#define GAME_MESSAGE_LOST_LIFE1 "You feel your life slipping away!"
#define GAME_MESSAGE_LOST_LIFE2 "You feel your life draining away..."
#define GAME_MESSAGE_BLOE_AWAY "You blow %s away!"
#define GAME_MESSAGE_CAMP_GET_ANGRY "%^s gets angry!"
#define GAME_MESSAGE_HOSTILE "%^s suddenly becomes hostile!"
#define GAME_MESSAGE_BOUNTY_DEAD "There is a price on %s's head."
#endif

#ifdef JP
#define GAME_MESSAGE_RESTORE_SOME_MP "少し頭がハッキリした。"
#else
#define GAME_MESSAGE_RESTORE_SOME_MP "You feel your head clear a little."
#endif

#ifdef JP
#define GAME_MESSAGE_EARTHQUAKE "地面が揺れた。"
#else
#define GAME_MESSAGE_EARTHQUAKE "The ground trembles."
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
#define GAME_MESSAGE_DETECT_NOLIVING "自然でないクリーチャーの存在を感じた！"
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
#define GAME_MESSAGE_DETECT_NOLIVING "You sense the presence of unnatural beings!"
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
#define MES_BLDG_CLOSED "(閉店)"
#define MES_BLDG_NO_BLDG "ここには建物はない。"
#define MES_BLDG_LOSER "「敗者に用はない。」"
#define MES_BLDG_ARENA_FIGHT "ゲートは閉まっている。相手があなたを待っている！"
#define MES_BLDG_NO_RIGHT "それを選択する権利はありません！"

#define MES_BLDG_GET_PRICE "賞金 %ld＄を手に入れた。"
#define MES_BLDG_TODAY_TARGET1(TARGET) "ターゲット： %s", (TARGET)
#define MES_BLDG_TODAY_TARGET2(PRICE) "死体 ---- $%d", (PRICE)
#define MES_BLDG_TODAY_TARGET3(PRICE) "骨   ---- $%d", (PRICE)
#define MES_BLDG_TSUCHINOKO1 "一獲千金の大チャンス！！！\nターゲット：幻の珍獣「ツチノコ」"
#define MES_BLDG_TSUCHINOKO2 "生け捕り ---- $1,000,000\n死体     ----   $200,000\n骨       ----   $100,000"

#else
#define MES_BLDG_CLOSED "(closed)"
#define MES_BLDG_NO_BLDG "You see no building here."
#define MES_BLDG_LOSER "'There's no place here for a LOSER like you!'"
#define MES_BLDG_ARENA_FIGHT "The gates are closed.  The enemy awaits!"
#define MES_BLDG_NO_RIGHT "You have no right to choose that!"

#define MES_BLDG_GET_PRICE "You get %ldgp."
#define MES_BLDG_TODAY_TARGET1(TARGET) "target: %s", (TARGET)
#define MES_BLDG_TODAY_TARGET2(PRICE) "corpse   ---- $%d", (PRICE)
#define MES_BLDG_TODAY_TARGET3(PRICE) "skeleton ---- $%d", (PRICE)
#define MES_BLDG_TSUCHINOKO1 "Big chance to quick money!!!\ntarget: the rarest animal 'Tsuchinoko'"
#define MES_BLDG_TSUCHINOKO2 "catch alive ---- $1,000,000\ncorpse      ----   $200,000\nbones       ----   $100,000"

#endif

#ifdef JP
#define GAME_MESSAGE_FEATURE_NO_UP_STAIR "ここには上り階段が見当たらない。"
#define GAME_MESSAGE_FEATURE_NO_DOWN_STAIR "ここには下り階段が見当たらない。"
#define GAME_MESSAGE_FEATURE_UP_STAIR "上の階に降りた。"
#define GAME_MESSAGE_FEATURE_DOWN_STAIR "下の階に降りた。"
#define GAME_MESSAGE_FEATURE_CREATE_STAIR "魔法の階段が現れた..."

#define GAME_MESSAGE_FEATURE_STUCK "%sはがっちりと閉じられているようだ。"
#define GAME_MESSAGE_FEATURE_CLOSED_DUNGEON "ダンジョンの入口は塞がれている！"
#define GAME_MESSAGE_FEATURE_CHECK_DUNGEON "ここには%sの入り口(%d階相当)があります"
#define GAME_MESSAGE_FEATURE_ENTER_DUNGEON "本当にこのダンジョンに入りますか？"

#define GAME_MESSAGE_PATTERN_LIMIT1 "パターンの上を歩き始めると、全てを歩かなければなりません。いいですか？"
#define GAME_MESSAGE_PATTERN_LIMIT2 "パターンの上を歩くにはスタート地点から歩き始めなくてはなりません。"
#define GAME_MESSAGE_PATTERN_LIMIT3 "パターンの上は正しい順序で歩かねばなりません。"
#define GAME_MESSAGE_PATTERN_LIMIT4 "パターンを踏み外してはいけません。"
#define GAME_MESSAGE_PATTERN_GOAL "「パターン」のこの部分は他の部分より強力でないようだ。"
#define GAME_MESSAGE_PATTERN_WRECKED "パターンを血で汚してしまった！何か恐ろしい事が起こりそうだ！"

#else
#define GAME_MESSAGE_FEATURE_NO_UP_STAIR "I see no up staircase here."
#define GAME_MESSAGE_FEATURE_NO_DOWN_STAIR "I see no down staircase here."
#define GAME_MESSAGE_FEATURE_UP_STAIR "You enter the up staircase."
#define GAME_MESSAGE_FEATURE_DOWN_STAIR "You enter the down staircase."
#define GAME_MESSAGE_FEATURE_CREATE_STAIR "A magical staircase appears..."

#define GAME_MESSAGE_FEATURE_STUCK "The %s appears to be stuck."
#define GAME_MESSAGE_FEATURE_CLOSED_DUNGEON "The entrance of this dungeon is closed!"
#define GAME_MESSAGE_FEATURE_CHECK_DUNGEON "There is the entrance of %s (Danger level: %d)"
#define GAME_MESSAGE_FEATURE_ENTER_DUNGEON "Do you really get in this dungeon? "

#define GAME_MESSAGE_PATTERN_LIMIT1 "If you start walking the Pattern, you must walk the whole way. Ok? "
#define GAME_MESSAGE_PATTERN_LIMIT2 "You must start walking the Pattern from the startpoint."
#define GAME_MESSAGE_PATTERN_LIMIT3 "You must walk the Pattern in correct order."
#define GAME_MESSAGE_PATTERN_LIMIT4 "You may not step off from the Pattern."
#define GAME_MESSAGE_PATTERN_GOAL "This section of the Pattern looks less powerful."
#define GAME_MESSAGE_PATTERN_WRECKED "You bleed on the Pattern! Something terrible happens!"
#endif

#ifdef JP
#define GAME_MESSAGE_SUMMON_SERVANT "御用でございますか？我が主。"
#else
#define GAME_MESSAGE_SUMMON_SERVANT "'What is thy bidding... Master?'"
#endif

#ifdef JP
#define GAME_MESSAGE_OBJECT_KILLED "壊れてしまった。"
#define GAME_MESSAGE_OBJECT_DUSTED "灰になった。"
#else
#define GAME_MESSAGE_OBJECT_KILLED (plural ? " are destroyed!" : " is destroyed!")
#define GAME_MESSAGE_OBJECT_DUSTED (plural ? " become dust." : " becomes dust.")
#endif

#ifdef JP
#define GAME_MESSAGE_DIGGING_PERMANENT1 "この岩は硬すぎて掘れないようだ。"
#define GAME_MESSAGE_DIGGING_PERMANENT2 "そこは掘れない!"
#define GAME_MESSAGE_DIGGED1(NAME) ("%sをくずした。", (NAME))
#define GAME_MESSAGE_DIGGED2(NAME) ("%sをくずしている。", (NAME))
#else
#define GAME_MESSAGE_DIGGING_PERMANENT1 "This seems to be permanent rock."
#define GAME_MESSAGE_DIGGING_PERMANENT2 "You can't tunnel through that!"
#define GAME_MESSAGE_DIGGED1(NAME) ("You have removed the %s.", (NAME))
#define GAME_MESSAGE_DIGGED2(NAME) ("You dig into the %s.", (NAME))
#endif

#ifdef JP
#define MES_TY_CURSE_TELEPORT "周囲の空間が歪んだ！"
#define MES_TY_CURSE_PURE_MANA "純粋な魔力の次元への扉が開いた！"
#define MES_TY_CURSE_SURGE_ENERGY "エネルギーのうねりを感じた！"
#define MES_TY_CURSE_STATUE "彫像になった気分だ！"
#define MES_TY_CURSE_AMNESIA "ほえ？私は誰？ここで何してる？"
#else
#define MES_TY_CURSE_TELEPORT "Space warps about you!"
#define MES_TY_CURSE_PURE_MANA "A portal opens to a plane of raw mana!"
#define MES_TY_CURSE_SURGE_ENERGY "A surge of energy happens!"
#define MES_TY_CURSE_STATUE "You feel like a statue!"
#define MES_TY_CURSE_AMNESIA "Huh? Who am I? What am I doing here?"
#endif

#if JP
#define MES_TELEPORT_LEVEL_UP(TARGET) "%^sは天井を突き破って宙へ浮いていく。", (TARGET)
#define MES_TELEPORT_LEVEL_DOWN(TARGET) "%^sは床を突き破って沈んでいく。", (TARGET)
#define MES_RECALL_STARTING "回りの大気が張りつめてきた..."
#define MES_RECALL_CANCEL "張りつめた大気が流れ去った..."
#else
#define MES_TELEPORT_LEVEL_UP(TARGET) "%^s rise%s up through the ceiling.", (TARGET), (m_idx <= 0) ? "" : "s"
#define MES_TELEPORT_LEVEL_DOWN(TARGET) "%^s sink%s through the floor.", m_name, (m_idx <= 0) ? "" : "s"
#define MES_RECALL_STARTING "The air about you becomes charged..."
#define MES_RECALL_CANCEL "A tension leaves the air around you..."
#endif

#if JP
#define MES_ALCHEMY_ASK(OBJECT) "本当に%sを金に変えますか？", (OBJECT)
#define MES_ALCHEMY_FAILED(OBJECT) "%sを金に変えることに失敗した。", (OBJECT)
#define MES_ALCHEMY_NO_PRICE(OBJECT) "%sをニセの金に変えた。", (OBJECT)
#define MES_ALCHEMY_SUCCESS(OBJECT, PRICE) "%sを＄%d の金に変えた。", (OBJECT), (PRICE)
#else
#define MES_ALCHEMY_ASK(OBJECT) "Really turn %s to gold? ", (OBJECT)
#define MES_ALCHEMY_FAILED(OBJECT) "You fail to turn %s to gold!", (OBJECT)
#define MES_ALCHEMY_NO_PRICE(OBJECT) "You turn %s to fool's gold.", (OBJECT)
#define MES_ALCHEMY_SUCCESS(OBJECT, PRICE) "You turn %s to %ld coins worth of gold.", (OBJECT), (PRICE)
#endif

#ifdef JP
#define MES_ENCHANT_CURSE_BROKEN "かけられていた呪いが打ち破られた！"
#else
#define MES_ENCHANT_CURSE_BROKEN "The curse is broken!"
#endif

#ifdef JP
#define MES_INDENTIFY_RESULT_EQUIPMENT(EQUIP_POS, NAME, LABEL) "%^s: %s(%c)。", (EQUIP_POS), (NAME), (LABEL)
#define MES_INDENTIFY_RESULT_IN_ZACK(NAME, LABEL) "ザック中: %s(%c)。", (NAME), (LABEL)
#define MES_INDENTIFY_RESULT_ON_GROUND(NAME) "床上: %s。", (NAME)
#else
#define MES_INDENTIFY_RESULT_EQUIPMENT(EQUIP_POS, NAME, LABEL) "%^s: %s(%c)。", (EQUIP_POS), (NAME), (LABEL)
#define MES_INDENTIFY_RESULT_IN_ZACK(NAME, LABEL) "In your pack: %s (%c).", (NAME), (LABEL)
#define MES_INDENTIFY_RESULT_ON_GROUND(NAME) "On the ground: %s.", (NAME)
#endif

// Cause of Death
#ifdef JP

#define COD_QUITTING "途中終了"
#define COD_INTERRUPTING "強制終了"
#define COD_CHEATING_DEATH "死の欺き"
#define COD_SAVE "(セーブ)"
#define COD_PANIC_SAVE "(緊急セーブ)"
#define COD_ALIVE_AND_WELL "(元気に生きている)"
#define COD_RETIRE "引退"
#define COD_SEPPUKU "切腹"
#define COD_SUICIDE_BOMBER "自爆"
#define COD_TRAP "トラップ"
#define COD_POISON "毒"
#define COD_WOUND "致命傷"
#define COD_STARVATION "飢餓"
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
#define COD_BLOOD_CURSE "血の呪いの自傷"
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
#define COD_TY_CURSE_ENERGY "エネルギーのうねり"
#define COD_UNC_FORCE "気の暴走" 
#define COD_CHAOS_TAINT "混沌に蝕まれたダメージ"
#define COD_PATRON_WRATH(PATRON) "%sの怒り", (PATRON)
#define COD_EARTHQUAKE_CASTER(CASTER) "%sの起こした地震", (CASTER)
#define COD_EARTHQUAKE "地震"
#define COD_PURE_MANA "純粋な魔力の解放"
#else

#define COD_QUITTING "Quitting"
#define COD_INTERRUPTING "Interrupting"
#define COD_CHEATING_DEATH "cheating death"
#define COD_SAVE "(saved)"
#define COD_PANIC_SAVE "(panic save)"
#define COD_ALIVE_AND_WELL "(alive and well)"
#define COD_RETIRE "retired"
#define COD_SEPPUKU "seppuku"
#define COD_SUICIDE_BOMBER "suicide bomber"
#define COD_TRAP "a trap"
#define COD_POISON "poison"
#define COD_WOUND "a fatal wound"
#define COD_STARVATION "starvation"
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
#define COD_BLOOD_CURSE "self wound for blood curse"
#define COD_BUNP_WALL "bumping into wall"
#define COD_DENSITY "density"
#define COD_SOLID_ROCK "solid rock"
#define COD_DROWNING "drowning"
#define COD_FIRE_AURA "fire aura"
#define COD_COLD_AURA "cold aura"
#define COD_ELEC_AURA "elec aura"
#define COD_LETHAL_MUTATION "a lethal mutation"
#define COD_POLYMORPHED_WOUND "a polymorphed wound"
#define COD_TY_CURSE_ENERGY "surge of energy"
#define COD_UNC_FORCE "Explosion of the Force"
#define COD_CHAOS_TAINT "Damage of tainted by chaos"
#define COD_PATRON_WRATH(PATRON) "the Wrath of %s", (PATRON)
#define COD_EARTHQUAKE_CASTER(CASTER) "an earthquake caused by %s", (CASTER)
#define COD_EARTHQUAKE "an earthquake"
#define COD_PURE_MANA "released pure mana"
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
#define BINARY_FILES_RAW(NAME) "%s_j.raw", (NAME)
#else
#define BINARY_FILES_RAW(NAME) "%s.raw", (NAME)
#endif

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
	#define PROMPT_DIRECTION "方向は(ESCで中断)? "
	#define PROMPT_LABEL(N1, N2) " %c-%c,'(',')',", (N1), (N2)
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
	#define PROMPT_LABEL(N1, N2) " %c-%c,'(',')',", (N1), (N2)
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
	#define SKILL_NAME_FORCE "錬気術"
	#define SKILL_NAME_BRUTAL "狂奔"
#else
	#define SKILL_NAME_MAGIC "magic"
	#define SKILL_NAME_PSIONIC "psionic powers"
	#define SKILL_NAME_IMITATION "imitation"
	#define SKILL_NAME_HISSATSU "hissatsu"
	#define SKILL_NAME_MIRROR_M "mirror magic"
	#define SKILL_NAME_NINJUTSU "ninjutsu"
	#define SKILL_NAME_PRAYER "prayer"
	#define SKILL_NAME_FORCE "force"
	#define SKILL_NAME_BRUTAL "brutal power"
#endif

#ifdef JP

#define DIARY_CREATE_PLAYER "-------- プレイヤー作成 --------"
#define DIARY_GAMEOVER "-------- ゲームオーバー --------"
#define DIARY_SAVEQUIT "----ゲーム中断----"
#define DIARY_GAMERESTART "----ゲーム再開----"
#define DIARY_GAMEABORT "----ゲーム異常終了----"
#define DIARY_SELECTMODE "キャンペーン『%s』を選択した"
#define DIARY_RETIRED "ダンジョンの探索から引退した。"
#define DIARY_SUICIDE "絶望して自殺した。"
#define DIARY_WINNING_SEPPUKU "勝利の後切腹した。"
#define DIARY_WIZARD "ウィザードモードに突入してスコアを残せなくなった。"
#define DIARY_UP_STAIR "階段を上った"
#define DIARY_DOWN_STAIR "階段を下りた"
#define DIARY_TRAP_DOOR "落とし戸に落ちた"
#define DIARY_PINCH "%sによってピンチに陥った。"
#define DIARY_PATRON(REWARD) "パトロンの報酬で%s", (REWARD)
#else

#define DIARY_CREATE_PLAYER "-------- Create Player --------"
#define DIARY_GAMEOVER "--------   Game  Over   --------"
#define DIARY_SAVEQUIT "---- Save and Exit Game ----"
#define DIARY_GAMERESTART "---- Restart Game ----"
#define DIARY_GAMEABORT "---- Panic Save and Abort Game ----"
#define DIARY_SELECTMODE "Select Campaign '%s'"
#define DIARY_RETIRED "retired exploring dungeons."
#define DIARY_SUICIDE "give up all hope to commit suicide."
#define DIARY_WINNING_SEPPUKU "did Seppuku after the winning."
#define DIARY_WIZARD "give up sending score to use wizard mode."
#define DIARY_UP_STAIR 1"climbed up the stairs to"
#define DIARY_DOWN_STAIR "climbed down the stairs to"
#define DIARY_TRAP_DOOR "You have fallen through a trap door."
#define DIARY_PINCH "A critical situation because of %s".
#define DIARY_PATRON(REWARD) "The patron rewards you with %s.", (REWARD)
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
#define DEBUG_MESSAGE_BIAS_ARTIFACT "運の偏ったアーティファクト: %d。"
#define DEBUG_MESSAGE_NO_BIAS_ARTIFACT "偏っていないアーティファクト"
#define DEBUG_MESSAGE_DIE_CHOICE "死にますか?"
#define DEBUG_MESSAGE_LAKE "湖を生成。"
#define DEBUG_MESSAGE_CAVE "洞窟を生成。"
#define DEBUG_MESSAGE_ARENA "アリーナレベル"
#define DEBUG_MESSAGE_DEC_CREATURE(FROM, TO) "クリーチャー数基本値を %d から %d に減らします", (FROM), (TO)
#define DEBUG_MESSAGE_SMALL_LEVEL "小さなフロア"
#define DEBUG_MESSAGE_FLOOR_SIZE(WIDTH, HEIGHT) "[W:%d, H:%d]"
#define DEBUG_MESSAGE_VAULT(NAME) "地下室(%s)", (NAME)
#define DEBUG_MESSAGE_GREATER_VAULT(NAME) "巨大な地下室(%s)", (NAME)
#define DEBUG_MESSAGE_FAILED_VAULT "巨大な地下室を配置できません。"
#define DEBUG_MESSAGE_ARTIFACT_CANCEL(OBJECT) "伝説のアイテム (%s) は生成中に*破壊*された。", (OBJECT)
#define DEBUG_MESSAGE_RANDOM_ARTIFACT_CANCEL "ランダム・アーティファクトの1つは生成中に*破壊*された。"
#define DEBUG_MESSAGE_ARTIFACT_CANCEL2(OBJECT) "伝説のアイテム (%s) はストリーマーにより削除された。", (OBJECT)
#define DEBUG_MESSAGE_RANDOM_ARTIFACT_CANCEL2 "ランダム・アーティファクトの1つはストリーマーにより削除された。"
#define DEBUG_MESSAGE_FAILED_STREAMER "ストリーマーの生成に失敗した。"
#define DEBUG_MESSAGE_DESTROYED_FLOOR "破壊された階"
#else
#define DEBUG_MESSAGE_ROOM_NUM "Number of Rooms: %d"
#define DEBUG_MESSAGE_GLASS_ROOM "[Glass Room]"
#define DEBUG_MESSAGE_ROOM_OF "[Room of %s]"
#define DEBUG_MESSAGE_CRYPT_ROOM "[Crypt]"
#define DEBUG_MESSAGE_TRAP_PIT_ROOM "[Trapped creature pit (%s%s)]"
#define DEBUG_MESSAGE_BIAS_ARTIFACT "Biased artifact: %d."
#define DEBUG_MESSAGE_NO_BIAS_ARTIFACT "No biased artifact."
#define DEBUG_MESSAGE_DIE_CHOICE "Die?"
#define DEBUG_MESSAGE_LAKE "Lake on the level."
#define DEBUG_MESSAGE_CAVE "Cavern on level."
#define DEBUG_MESSAGE_ARENA "Arena level."
#define DEBUG_MESSAGE_DEC_CREATURE(FROM, TO) "Reduced creatures base from %d to %d", (FROM), (TO)
#define DEBUG_MESSAGE_SMALL_LEVEL "A small dungeon level."
#define DEBUG_MESSAGE_FLOOR_SIZE(WIDTH, HEIGHT) "[W:%d, H:%d]"
#define DEBUG_MESSAGE_VAULT(NAME) "Vault(%s)", (NAME)
#define DEBUG_MESSAGE_GREATER_VAULT(NAME) "Greater vault (%s)", (NAME)
#define DEBUG_MESSAGE_FAILED_VAULT "Could not place greater vault."
#define DEBUG_MESSAGE_ARTIFACT_CANCEL(OBJECT) "Artifact (%s) was *destroyed* during generation.", (OBJECT)
#define DEBUG_MESSAGE_RANDOM_ARTIFACT_CANCEL "One of the random artifacts was *destroyed* during generation."
#define DEBUG_MESSAGE_ARTIFACT_CANCEL2(OBJECT) "Artifact (%s) was deleted by streamer.", (OBJECT)
#define DEBUG_MESSAGE_RANDOM_ARTIFACT_CANCEL2 "One of the random artifacts was deleted by streamer."
#define DEBUG_MESSAGE_FAILED_STREAMER "Could not place streamer!"
#define DEBUG_MESSAGE_DESTROYED_FLOOR "Destroyed Floor"
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

#ifdef JP
#define MES_BIRTH_SELECT_CAMPAIGN "キャンペーンを選択して下さい"
#else
#define MES_BIRTH_SELECT_CAMPAIGN "Select Campaign"
#endif

#ifdef JP
#define MES_QUEST_INFO(LEVEL) "クエスト情報 (危険度: %d 階相当)", (LEVEL)
#define MES_QUEST_NO_QUEST "今のところクエストはありません。"
#define MES_QUEST_DEFAULT_KILLING "クエスト: %sを %d体倒す"
#define MES_QUEST_WARN_UNCOMPLETE "あなたは現在のクエストを終了させていません！\nCTRL-Qを使えばクエストの状態がチェックできます。\nクエストを終わらせたら戻って来て下さい。\n"
#define MES_QUEST_ASK_ACCEPT "このクエストを受諾しますか？"
#define MES_QUEST_ACCEPT "クエスト『%s』を受諾しました。"
#define MES_QUEST_NO_ENTRANCE "ここにはクエストの入口はない"
#define MES_QUEST_ENTRANCE "ここにはクエストへの入口があります。"
#define MES_QUEST_ASK_ENTER "クエストに入りますか？"
#define MES_QUEST_TARGET_INFO(TARGET) "注意せよ！この階は%sによって守られている！", (TARGET)
#define MES_QUEST_TARGETS_INFO(TARGET, NUM) "注意しろ！この階は%d体の%sによって守られている！", (NUM), (TARGET)
#define MES_QUEST_EXIT_WARNING "この階を一度去ると二度と戻って来られません。"
#define MES_QUEST_ASK_EXIT "本当にこの階を去りますか？"
#else
#define MES_QUEST_INFO(LEVEL) ("Quest Information (Danger level: %d)", (LEVEL))
#define MES_QUEST_NO_QUEST "I don't have a quest for you at the moment."
#define MES_QUEST_DEFAULT_KILLING "Your quest: kill %d %s"
#define MES_QUEST_WARN_UNCOMPLETE "You have not completed your current quest yet!\nUse CTRL-Q to check the status of your quest.\nReturn when you have completed your quest."
#define MES_QURST_ASK_ACCEPT "Accept this quest? "
#define MES_QUEST_ACCEPT "You accepted the quest, '%s'. "
#define MES_QUEST_NO_ENTRANCE "You see no quest level here."
#define MES_QUEST_ENTRANCE "There is an entry of a quest."
#define MES_QUEST_ASK_ENTER "Do you enter? "
#define MES_QUEST_TARGET_INFO(TARGET) "Beware, this level is protected by %s!", (TARGET)
#define MES_QUEST_TARGETS_INFO(TARGET, NUM) "Be warned, this level is guarded by %d %s!", (NUM), (TARGET)
#define MES_QUEST_EXIT_WARNING "You can't come back here once you leave this floor."
#define MES_QUEST_ASK_EXIT "Really leave this floor? "
#endif

static cptr desc_stat_pos[] =
{
#ifdef JP
	"強く",
	"知的に",
	"賢く",
	"器用に",
	"健康に",
	"美しく"
#else
	"strong",
	"smart",
	"wise",
	"dextrous",
	"healthy",
	"cute"
#endif
};


static cptr desc_stat_neg[] =
{
#ifdef JP
	"弱く",
	"無知に",
	"愚かに",
	"不器用に",
	"不健康に",
	"醜く"
#else
	"weak",
	"stupid",
	"naive",
	"clumsy",
	"sickly",
	"ugly"
#endif
};

#ifdef JP
#define MES_CREATURE_LEVELUP(LEV) "レベル %d にようこそ。", (LEV)
#define MES_CREATURE_DEC_STATUS(CREATURE, DEC) "%sはひどく%sなった。", (CREATURE)->name, (DEC)
#define MES_CREATURE_DEC_STATUS_CANCEL(CREATURE, DEC) "%sはひどく%sなりかけたが、元に戻った", (CREATURE)->name, (DEC)
#define MES_CREATURE_LIFE_RATE(NUM) "現在の体力ランク : %d/100", (NUM)
#else
#define MES_CREATURE_LEVELUP(LEV) "Welcome to level %d.", (LEV)
#define MES_CREATURE_DEC_STATUS(CREATURE, DEC) "%s become very %s.", (CREATURE)->name, (DEC)
#define MES_CREATURE_DEC_STATUS_CANCEL(CREATURE, DEC) "%s become %s for a moment, but the feeling passes.", (CREATURE)->name, (DEC)
#define MES_CREATURE_LIFE_RATE(NUM) "Your current Life Rating is %d/100.", (NUM)
#endif

#ifdef JP
#define MES_POLYSELF_FEELING "あなたは変化の訪れを感じた..."
#define MES_POLYSELF_ORDINARY "奇妙なくらい普通になった気がする。"
#define MES_POLYSELF_DIFFICULT_FORM "現在の姿で生きていくのは困難なようだ！"
#else
#define MES_POLYSELF_FEELING "You feel a change coming over you..."
#define MES_POLYSELF_ORDINARY "You feel oddly normal."
#define MES_POLYSELF_DIFFICULT_FORM "You find living difficult in your present form!"
#endif

#ifdef JP
#define MES_STORE_ENTIRE "これで全部です。"
#define MES_STORE_ITEM_LIMIT "そんなにアイテムを持てない。"
#define MES_STORE_ITEM_LIMIT2 "ザックにそのアイテムを入れる隙間がない。"
#define MES_STORE_MUSEUM "博物館から取り出すことはできません。"
#define MES_STORE_NO_ITEM_HOME "我が家には何も置いてありません。"
#define MES_STORE_NO_ITEM_MUSEUM "博物館には何も置いてありません。"
#define MES_STORE_NO_ITEM_STORE "現在商品の在庫を切らしています。"
#define MES_STORE_WHICH_EXAMINE "どれを調べますか？"
#define MES_STORE_FULL_ITEM_HOME "我が家にはもう置く場所がない。"
#define MES_STORE_FULL_ITEM_MUSEUM "博物館はもう満杯だ。"
#define MES_STORE_FULL_ITEM_STORE "すいませんが、店にはもう置く場所がありません。"
#define MES_STORE_WHICH_TAKING "どのアイテムを取り出しますか？"
#define MES_STORE_WHICH_PURCHASE "どの品物が欲しいんだい？"
#define MES_STORE_WHICH_DROP "どのアイテムを置きますか? "
#define MES_STORE_WHICH_GIVE "どのアイテムを寄贈しますか? "
#define MES_STORE_WHICH_SELL "どのアイテムを売りますか? "
#define MES_STORE_WHICH_REMOVE "どのアイテムの展示をやめさせますか？"
#define MES_STORE_NO_DROP "置けるアイテムを持っていません。"
#define MES_STORE_NO_GIVE "寄贈できるアイテムを持っていません。"
#define MES_STORE_NO_SELL "欲しい物がないですねえ。"
#define MES_STORE_MUSEUM_SAME "それと同じ品物は既に博物館にあるようです。"
#define MES_STORE_MUSEUM_WARN "博物館に寄贈したものは取り出すことができません！！"
#define MES_STORE_MUSEUM_WARN2 "展示をやめさせたアイテムは二度と見ることはできません！"
#define MES_STORE_MUSEUM_ASK(OBJECT) "本当に%sを寄贈しますか？", (OBJECT)
#define MES_STORE_MUSEUM_ASK2(OBJECT) "本当に%sの展示をやめさせますか？", (OBJECT)
#define MES_STORE_FIXED_PRICE(PRICE) "一つにつき $%ldです。", (PRICE)
#define MES_STORE_BUYING(NAME, SYM) "%s(%c)を購入する。", (NAME), (SYM)
#define MES_STORE_SELLING(NAME, SYM) "%s(%c)を売却する。", (NAME), (SYM)
#define MES_STORE_BOUGHT(NAME, PRICE) "%sを $%ldで購入しました。", (NAME), (PRICE)
#define MES_STORE_SELL(NAME, PRICE) "%sを $%ldで売却しました。", (NAME), (PRICE)
#define MES_STORE_NO_FIT "あなたの体格に合わないようだが、よろしいか？"
#define MES_STORE_NEW_STOCK "店主は新たな在庫を取り出した。"
#define MES_STORE_PURCHASED(NAME, SYM) "%s(%c)を手に入れた。", (NAME), (SYM)
#define MES_STORE_TOOK(NAME, SYM) "%s(%c)を取り出した。", (NAME), (SYM)
#define MES_STORE_DROP(NAME, SYM) "%sを置いた。(%c)", (NAME), (SYM)
#define MES_STORE_REMOVE(NAME) "%sの展示をやめさせた。", (NAME)
#define MES_STORE_COMMAND_LIMIT "そのコマンドは店の中では使えません。"
#define MES_STORE_COMMAND_LIST1 " -)前ページ"
#define MES_STORE_COMMAND_LIST2 " スペース) 次ページ"
#define MES_STORE_COMMAND_LIST3 " g) アイテムを取る"
#define MES_STORE_COMMAND_LIST4 " d) アイテムを置く"
#define MES_STORE_COMMAND_LIST5 " x) 家のアイテムを調べる"
#define MES_STORE_COMMAND_LIST6 " r) アイテムの展示をやめる"
#define MES_STORE_COMMAND_LIST7 " p) 商品を買う"
#define MES_STORE_COMMAND_LIST8 " s) アイテムを売る"
#define MES_STORE_COMMAND_LIST9 " x) 商品を調べる"
#define MES_STORE_COMMAND_LIST10 " l) 店主の様子を伺う"
#define MES_STORE_COMMAND_LIST11 " i/e) 持ち物/装備の一覧"
#define MES_STORE_COMMAND_LIST12 " w/T) 装備する/はずす"
#define MES_STORE_COMMAND_LIST13 " w/t) 装備する/はずす"
#define MES_STORE_COMMAND_LIST14 " ESC) 店を出る"
#define MES_STORE_OVERFLOW1 "ザックからアイテムがあふれそうなので、あわてて博物館から出た..."
#define MES_STORE_OVERFLOW2 "ザックからアイテムがあふれそうなので、あわてて店から出た..."
#define MES_STORE_OVERFLOW3 "ザックからアイテムがあふれそうなので、あわてて家から出た..."
#define MES_STORE_DROP_ITEM(NAME, SYM) "%sが落ちた。(%c)", (NAME), (SYM)
#else
#define MES_STORE_ENTIRE "That's all."
#define MES_STORE_ITEM_LIMIT "You cannot carry that many different items."
#define MES_STORE_ITEM_LIMIT2 "You cannot carry that many items."
#define MES_STORE_MUSEUM "Museum."
#define MES_STORE_NO_ITEM_HOME "Your home is empty."
#define MES_STORE_NO_ITEM_MUSEUM "Museum is empty."
#define MES_STORE_NO_ITEM_STORE "I am currently out of stock."
#define MES_STORE_WHICH_EXAMINE "Which item do you want to examine? "
#define MES_STORE_FULL_ITEM_HOME "Your home is full."
#define MES_STORE_FULL_ITEM_MUSEUM "Museum is full."
#define MES_STORE_FULL_ITEM_STORE "I have not the room in my store to keep it."
#define MES_STORE_WHICH_TAKING "Which item do you want to take? "
#define MES_STORE_WHICH_PURCHASE "Which item are you interested in? "
#define MES_STORE_WHICH_DROP "Drop which item? "
#define MES_STORE_WHICH_GIVE "Give which item? "
#define MES_STORE_WHICH_SELL "Sell which item? "
#define MES_STORE_WHICH_REMOVE "Which item do you want to order to remove? "
#define MES_STORE_NO_DROP "You don't have any item to drop."
#define MES_STORE_NO_GIVE "You don't have any item to give."
#define MES_STORE_NO_SELL "You have nothing that I want."
#define MES_STORE_MUSEUM_SAME "The same object as it is already in the Museum."
#define MES_STORE_MUSEUM_WARN "You cannot take items which is given to the Museum back!!"
#define MES_STORE_MUSEUM_WARN2 "You cannot see items which is removed from the Museum!"
#define MES_STORE_MUSEUM_ASK(OBJECT) "Really give %s to the Museum? ", (OBJECT)
#define MES_STORE_MUSEUM_ASK2(OBJECT) "Really order to remove %s from the Museum? ", (OBJECT)
#define MES_STORE_FIXED_PRICE(PRICE) "That costs %ld gold per item.", (PRICE)
#define MES_STORE_BUYING(NAME, SYM) "Buying %s(%c).", (NAME), (SYM)
#define MES_STORE_SELLING(NAME, SYM) "Selling %s (%c).", (NAME), (SYM)
#define MES_STORE_BOUGHT(NAME, PRICE) "You bought %s for %ld gold.", (NAME), (PRICE)
#define MES_STORE_SELL(NAME, PRICE) "You sold %s for %ld gold.", (NAME), (PRICE)
#define MES_STORE_NO_FIT "It may be not your size, Are you sure?"
#define MES_STORE_NEW_STOCK "The shopkeeper brings out some new stock."
#define MES_STORE_PURCHASED(NAME, SYM) "You have %s (%c).", (NAME), (SYM)
#define MES_STORE_TOOK(NAME, SYM) "You have %s (%c).", (NAME), (SYM)
#define MES_STORE_DROP(NAME, SYM) "You drop %s (%c).", (NAME), (SYM)
#define MES_STORE_REMOVE(NAME) "You ordered to remove %s.", (NAME)
#define MES_STORE_COMMAND_LIMIT "That command does not work in stores."
#define MES_STORE_COMMAND_LIST1 " -) Previous page"
#define MES_STORE_COMMAND_LIST2 " SPACE) Next page"
#define MES_STORE_COMMAND_LIST3 " g) Get an item."
#define MES_STORE_COMMAND_LIST4 " d) Drop an item."
#define MES_STORE_COMMAND_LIST5 " x) eXamine an item in the home."
#define MES_STORE_COMMAND_LIST6 " r) order to Remove an item."
#define MES_STORE_COMMAND_LIST7 " p) Purchase an item."
#define MES_STORE_COMMAND_LIST8 " s) Sell an item."
#define MES_STORE_COMMAND_LIST9 " x) eXamine an item in the shop"
#define MES_STORE_COMMAND_LIST10 " l) Lock at the owner"
#define MES_STORE_COMMAND_LIST11 " i/e) Inventry/Equipment list"
#define MES_STORE_COMMAND_LIST12 " w/T) Wear/Take off equipment"
#define MES_STORE_COMMAND_LIST13 " w/t) Wear/Take off equipment"
#define MES_STORE_COMMAND_LIST14 " ESC) Exit from Store."
#define MES_STORE_OVERFLOW1 "Your pack is so full that you flee the Museum..."
#define MES_STORE_OVERFLOW2 "Your pack is so full that you flee the store..."
#define MES_STORE_OVERFLOW3 "Your pack is so full that you flee your home..."
#define MES_STORE_DROP_ITEM(NAME, SYM) "You drop %s (%c).", (NAME), (SYM)
#endif

#ifdef JP
#define MES_STATUS_CUMBER_GROVE "手が覆われて呪文が唱えにくい感じがする。"
#define MES_STATUS_NO_CUMBER_GROVE "この手の状態なら、ぐっと呪文が唱えやすい感じだ。"
#define MES_STATUS_CUMBER_ARMOR "装備の重さで動きが鈍くなってしまっている。"
#define MES_STATUS_NO_CUMBER_ARMOR "ぐっと楽に体を動かせるようになった。"
#else
#define MES_STATUS_CUMBER_GROVE "Your covered hands feel unsuitable for spellcasting."
#define MES_STATUS_NO_CUMBER_GROVE "Your hands feel more suitable for spellcasting."
#define MES_STATUS_CUMBER_ARMOR "The weight of your equipment encumbers your movement."
#define MES_STATUS_NO_CUMBER_ARMOR "You feel able to move more freely."
#endif

#ifdef JP
#define MES_PATRON_BOOM_OUT(PATRON) "%sの声が響き渡った:", (PATRON)
#else
#define MES_PATRON_BOOM_OUT(PATRON) "The voice of %s booms out:", (PATRON)
#endif

#ifdef JP
#define MES_INTERFACE_DUMP "['h'でモード変更, ESCで終了]"
#else
#define MES_INTERFACE_DUMP "['h' to change mode, or ESC]"
#endif

#ifdef JP
#define MES_MELEE_ATTACK "%sは%sを%sで攻撃した。"
#define MES_MELEE_ATTACK_MISS "%sは%sの攻撃をかわした。"
#define MES_MELEE_AMBUSH(ATTACKER, TARGET) ("%sは冷酷にも眠っている無力な%sを突き刺した！", (ATTACKER), (TARGET))
#define MES_MELEE_FATAL_SPOT(ATTACKER, TARGET) ("%sは不意を突いて%sに強烈な一撃を喰らわせた！", (ATTACKER), (TARGET))
#define MES_MELEE_BACKSTUB(ATTACKER, TARGET) ("%sは逃げる%sを背中から突き刺した！", (ATTACKER), (TARGET))
#define MES_MELEE_ZANTETSU_JELLY "こんな軟らかいものは切れん！"
#define MES_MELEE_ZANTETSU_SERIF "またつまらぬものを斬ってしまった．．．"
#define MES_MELEE_EX_JR_SPIDER "蜘蛛は苦手だ！"
#define MES_MELEE_VORPAL_BLADE_SERIF "目にも止まらぬヴォーパルブレード、手錬の早業！"
#define MES_MELEE_ATTACK_DRAIN(WEAPON, TARGET) ("%sが%sから生命力を吸い取った！", (WEAPON), (TARGET))
#define MES_MELEE_SUCK_BLOOD(WEAPON) ("%sは血を吸って強くなった！", WEAPON)
#define MES_MELEE_RETURN(WEAPON, ATTACKER) ("振り回した%sが%sに返ってきた。", (WEAPON), (ATTACKER))
#define MES_MELEE_TO_EMPTY "何もない空中を攻撃した。"
#define MES_MELEE_DISABLE(ATTACKER) "%sは攻撃できない。", (ATTACKER)
#define MES_MELEE_NO_METHOD(ATTACKER) "%sは攻撃する手段を持っていない。", (ATTACKER)
#define MES_MELEE_PROTECTED(ATTACKER) "%^sは撃退された。", (ATTACKER)
#define MES_MELEE_COUNTER(ATTACKER) "%^sに反撃した！", (ATTACKER)
#define MES_MELEE_ZANTETSU_CANCEL(ATTACKER) "%sは思わず叫んだ。「拙者、おなごは斬れぬ！」", (ATTACKER)
#define MES_MELEE_THIEF "泥棒は笑って逃げた！"
#define MES_MELEE_THIEF_FAILED "泥棒は笑って逃げ...ようとしたがバリアに防がれた。"
#define MES_MELEE_NO_MELEE_DUNGEON "なぜか攻撃することができない。"
#define MES_MELEE_FORCED_STORMBRINGER(WEAPON, TARGET) "%sは強欲に%sを攻撃した！", (WEAPON), (TARGET)
#define MES_MELEE_ASK "本当に攻撃しますか？"
#define MES_MELEE_CEASE(TARGET) "%sを攻撃するのを止めた。", (TARGET)
#define MES_MELEE_FIRE_AURA "突然とても熱くなった！"
#define MES_MELEE_COLD_AURA "突然とても寒くなった！"
#define MES_MELEE_ELEC_AURA "電撃をくらった！"
#else
#define MES_MELEE_ATTACK_MISS %^s misses %s.
#define MES_MELEE_AMBUSH(ATTACKER, TARGET) "%s (cruelly stab the helpless, sleeping %s!", (ATTACKER), (TARGET))
#define MES_MELEE_FATAL_SPOT(ATTACKER, TARGET) ("%s make surprise attack, and hit %s with a powerful blow!", (ATTACKER), (TARGET))
#define MES_MELEE_BACKSTUB(ATTACKER, TARGET) ("%s ambush the fleeing %s.", (ATTACKER), (TARGET))
#define MES_MELEE_ZANTETSU_JELLY "You cannot cut such a elastic thing!"
#define MES_MELEE_ZANTETSU_SERIF "Sigh... Another trifling thing I've cut...."
#define MES_MELEE_EX_JR_SPIDER "Spiders are difficult for you to deal with!"
#define MES_MELEE_VORPAL_BLADE_SERIF "Your Vorpal Blade goes snicker-snack!"
#define MES_MELEE_SUCK_BLOOD(WEAPON) ("%s sucked blood, and became more powerful!", (WEAPON))
#define MES_MELEE_RETURN(WEAPON, ATTACKER) ("%s returns to %s.", ATTACKER, WEAPON)
#define MES_MELEE_TO_EMPTY "You attack the empty air."
#define MES_MELEE_DISABLE(ATTACKER) "%s don't have attack method.", (ATTACKER)
#define MES_MELEE_NO_METHOD(ATTACKER) "%s don't have attack method.", (ATTACKER)
#define MES_MELEE_PROTECTED(ATTACKER) "%^s is repelled.", (ATTACKER)
#define MES_MELEE_COUNTER(ATTACKER) "Your counterattack to %s!", (ATTACKER)
#define MES_MELEE_ZANTETSU_CANCEL(ATTACKER) "%s shouted, \"I can not attack women!\"", (ATTACKER)
#define MES_MELEE_THIEF "The thief flees laughing!"
#define MES_MELEE_THIEF_FAILED "The thief flees laughing...? But magic barrier obstructs it."
#define MES_MELEE_NO_MELEE_DUNGEON "Something prevent you from attacking."
#define MES_MELEE_FORCED_STORMBRINGER(WEAPON, TARGET) "%s greedily attacks %s!", (WEAPON), (TARGET)
#define MES_MELEE_ASK "Really hit it? "
#define MES_MELEE_CEASE(TARGET) "You stop to avoid hitting %s.", (TARGET)
#define MES_MELEE_FIRE_AURA "You are suddenly very hot!"
#define MES_MELEE_COLD_AURA "You are suddenly very cold!"
#define MES_MELEE_ELEC_AURA "You get zapped!"
#endif

#ifdef JP
#define MES_SHOOT_MARK(ARROW) "%sが敵を捕捉した。", (ARROW)
#else
#define MES_SHOOT_MARK(ARROW) "The %s finds a mark.", (ARROW)
#endif

#ifdef JP
#define MES_THROW_FIGURE_FAILED "人形は捻じ曲がり砕け散ってしまった！"
#define MES_THROW_FIGURE_CURSED "これはあまり良くない気がする。"
#else
#define MES_THROW_FIGURE_FAILED "The Figurine writhes and then shatters."
#define MES_THROW_FIGURE_CURSED "You have a bad feeling about this."
#endif

#ifdef JP
#define MES_OPEN_NO_TARGET "そこには開けるものが見当たらない。"
#else
#define MES_OPEN_NO_TARGET "You see nothing there to open."
#endif

#ifdef JP
#define MES_STUDY_WHICH_BOOK "どの本から学びますか? "
#define MES_STUDY_NO_BOOK "読める本がない。"
#define MES_STUDY_NO_READING "本を読むことができない！"
#define MES_STUDY_NO_SLOT(REALM) "新しい%sを覚えることはできない！", (REALM)
#define MES_STUDY_SLOT(REALM, NUM) "あと %d 種の%sを学べる。", (NUM), (REALM)
#define MES_STUDY_NO_SPELL(REALM) "その本には学ぶべき%sがない。", (REALM)
#define MES_STUDY_PERFECT_LEARNED(REALM) "その%sは完全に使いこなせるので学ぶ必要はない。", (REALM)
#else
#define MES_STUDY_NO_READING "You cannot read books!"
#define MES_STUDY_NO_SLOT(REALM) "You cannot learn any new %ss!", (REALM)
#define MES_STUDY_SLOT(REALM, NUM) "You can learn %d new %s%s.", (REALM), (NUM), ((NUM) == 1 ? "":"s")
#define MES_STUDY_NO_SPELL(REALM) "You cannot learn any %ss in that book.", (REALM)
#define MES_STUDY_PERFECT_LEARNED(REALM) "You don't need to study this %s anymore.", (REALM)
#endif

#ifdef JP
#define MES_RIDING_NO_DIRECTION "そちらには降りられません。"
#define MES_RIDING_NO_CREATURE "その場所にはクリーチャーはいません。"
#define MES_RIDING_NOT_PET "そのクリーチャーはペットではありません。"
#define MES_RIDING_DISABLE "そのクリーチャーには乗れなさそうだ。"
#else
#define MES_RIDING_NO_DIRECTION "You cannot go to that direction."
#define MES_RIDING_NO_CREATURE "Here is no creature."
#define MES_RIDING_NOT_PET "That creature is not a pet."
#define MES_RIDING_DISABLE "This creature doesn't seem suitable for riding."
#endif

#ifdef JP
#define MES_AUTOPICK_PATTERN_NOT_FOUND(STR) "パターンが見つかりません: %s", (STR)
#define MES_AUTOPICK_SKIP_INACTIVE_LINE(STR) "無効状態の行をスキップしました。(%sを検索中)", (STR)
#define MES_AUTOPICK_FOUND_ONLY_INACTIVE(STR) "無効状態の行だけが見付かりました。(%sを検索中)", (STR)
#define MES_AUTOPICK_NO_PATTERN_SEARCH "検索するパターンがありません(^S で検索)。"
#define MES_AUTOPICK_COMMENT "この行はコメントです。"
#define MES_AUTOPICK_CONDITIONAL_EXPRESSION "この行は条件分岐式です。"
#define MES_AUTOPICK_DEFINE_MACRO_ACTION "この行はマクロの実行内容を定義します。"
#define MES_AUTOPICK_DEFINE_TRIGER_KEY "この行はマクロのトリガー・キーを定義します。"
#define MES_AUTOPICK_DEFINE_KEY_MAP "この行はキー配置を定義します。"
#define MES_AUTOPICK_CUR_FALSE "現在の式の値は「偽(=0)」です。"
#define MES_AUTOPICK_CUR_TRUE "現在の式の値は「真(=1)」です。"
#define MES_AUTOPICK_CUR_DELETE "この行は後で削除されます。"
#define MES_AUTOPICK_CUR_BYPASS "この行は現在は無効な状態です。"
#else
#define MES_AUTOPICK_PATTERN_NOT_FOUND(STR) "Pattern not found: %s", (STR)
#define MES_AUTOPICK_SKIP_INACTIVE_LINE(STR) "Some inactive lines are skipped. (Searching %s)", (STR)
#define MES_AUTOPICK_FOUND_ONLY_INACTIVE(STR) "Found only an inactive line. (Searching %s)", (STR)
#define MES_AUTOPICK_NO_PATTERN_SEARCH "No pattern to search. (Press ^S to search.)"
#define MES_AUTOPICK_COMMENT "This line is a comment."
#define MES_AUTOPICK_CONDITIONAL_EXPRESSION "This line is a Conditional Expression."
#define MES_AUTOPICK_DEFINE_MACRO_ACTION "This line defines a Macro action."
#define MES_AUTOPICK_DEFINE_TRIGER_KEY "This line defines a Macro trigger key."
#define MES_AUTOPICK_DEFINE_KEY_MAP "This line defines a Keymap."
#define MES_AUTOPICK_CUR_FALSE "The expression is 'False'(=0) currently."
#define MES_AUTOPICK_CUR_TRUE "The expression is 'True'(=1) currently."
#define MES_AUTOPICK_CUR_DELETE "This line will be delete later."
#define MES_AUTOPICK_CUR_BYPASS "This line is bypassed currently."
#endif

#ifdef JP
#define MES_CRITICAL_LEVEL1 "手ごたえがあった！"
#define MES_CRITICAL_LEVEL2 "かなりの手ごたえがあった！"
#define MES_CRITICAL_LEVEL3 "会心の一撃だ！"
#define MES_CRITICAL_LEVEL4 "最高の会心の一撃だ！"
#define MES_CRITICAL_LEVEL5 "比類なき最高の会心の一撃だ！"
#else
#define MES_CRITICAL_LEVEL1 "It was a good hit!"
#define MES_CRITICAL_LEVEL2 "It was a great hit!"
#define MES_CRITICAL_LEVEL3 "It was a superb hit!"
#define MES_CRITICAL_LEVEL4 "It was a *GREAT* hit!"
#define MES_CRITICAL_LEVEL5 "It was a *SUPERB* hit!"
#endif

#ifdef JP
#define MES_BLDG_RECHARGE_WHICH_ITEM "どのアイテムに魔力を充填しますか? "
#define MES_BLDG_RECHARGE_NO_ITEM "魔力を充填すべきアイテムがない。"
#define MES_BLDG_RECHARGE_NO_NEED "それは再充填する必要はありません。"
#define MES_BLDG_RECHARGE_NO_MONEY(NAME, MONEY) "%sを再充填するには＄%d 必要です！", (NAME), (MONEY)
#define MES_BLDG_RECHAEGED(NAME, MONEY, NUM) "%sを＄%d で再充填しました。", (NAME), (MONEY), (NUM)
#define MES_BLDG_RECHAEGED_ALL(MONEY) "＄%d で再充填しました。", (MONEY)
#define MES_RECHAGE_FAILED(NAME) "魔力が逆流した！%sは完全に魔力を失った。", (NAME)
#define MES_RECHAGE_FAILED2 "魔力が逆噴射して、ロッドからさらに魔力を吸い取ってしまった！"
#define MES_RECHAGE_FAILED3(NAME) "%sは破損を免れたが、魔力が全て失われた。", (NAME)
#define MES_RECHAGE_BROKEN1(NAME) "乱暴な魔法のために%sが壊れた！", (NAME)
#define MES_RECHAGE_BROKEN2(NAME) "乱暴な魔法のために%sが一本壊れた！", (NAME)
#define MES_RECHAGE_BROKEN3(NAME) "乱暴な魔法のために%sが全て壊れた！", (NAME)
#else
#define MES_BLDG_RECHARGE_WHICH_ITEM "Recharge which item? "
#define MES_BLDG_RECHARGE_NO_ITEM "You have nothing to recharge."
#define MES_BLDG_RECHARGE_NO_NEED "That doesn't need to be recharged."
#define MES_BLDG_RECHARGE_NO_MONEY(NAME, MONEY) "You need %d gold to recharge %s!", (MONEY), (NAME)
#define MES_BLDG_RECHAEGED(NAME, MONEY, NUM) "%^s %s recharged for %d gold.", (NAME), ((NUM)> 1) ? "were" : "was"), (MONEY) 
#define MES_BLDG_RECHAEGED_ALL(MONEY) "You pay %d gold.", (MONEY)
#define MES_RECHAGE_FAILED(NAME) "The recharging backfires - %s is completely drained!", (NAME)
#define MES_RECHAGE_FAILED2 "The recharge backfires, draining the rod further!"
#define MES_RECHAGE_FAILED3(NAME) "You save your %s from destruction, but all charges are lost.", (NAME)
#define MES_RECHAGE_BROKEN1(NAME) "Wild magic consumes your %s!", (NAME)
#define MES_RECHAGE_BROKEN2(NAME) "Wild magic consumes one of your %s!", (NAME)
#define MES_RECHAGE_BROKEN3(NAME) "Wild magic consumes all your %s!", (NAME)
#endif

#ifdef JP
#define MES_WEP_BLESS_WHICH_OBJECT "どのアイテムを祝福しますか？"
#define MES_WEP_BLESS_NO_OBJECT "祝福できる武器がありません。"
#define MES_WEP_BLESS_CURSED_CANCEL(NAME, ID) "%sを覆う黒いオーラは祝福を跳ね返した！", (NAME)
#define MES_WEP_BLESS_VANISH_CURSE(NAME, ID) "%s から邪悪なオーラが消えた。", (NAME)
#define MES_WEP_BLESS_RESIST "その武器は祝福を嫌っている！"
#define MES_WEP_BLESS_FAILED "周囲が凡庸な雰囲気で満ちた..."
#else
#define MES_WEP_BLESS_WHICH_OBJECT "Bless which weapon? "
#define MES_WEP_BLESS_NO_OBJECT "You have weapon to bless."
#define MES_WEP_BLESS_CURSED_CANCEL(NAME, ID) "The black aura on %s %s disrupts the blessing!", (((ID) >= 0) ? "your" : "the"), (NAME)
#define MES_WEP_BLESS_VANISH_CURSE(NAME, ID) "A malignant aura leaves %s %s.", (((ID) >= 0) ? "your" : "the"), (NAME)
#define MES_WEP_BLESS_RESIST "The weapon resists your blessing!"
#define MES_WEP_BLESS_FAILED "There is a static feeling in the air..."
#endif

#ifdef JP
#define MES_INSANITY_FACED(ADJ, ELDRITCH) "%s%sの顔を見てしまった！", (ADJ), (ELDRITCH)
#define MES_INSANITY_GLANCE(ADJ, ELDRITCH) "%s%sの顔が垣間見えた。", (ADJ), (ELDRITCH)
#else
#define MES_INSANITY_FACED(ADJ, ELDRITCH) "You behold the %s visage of %s.", (ADJ), (ELDRITCH)
#define MES_INSANITY_GLANCE(ADJ, ELDRITCH) "You glance at the %s visage of %s.", (ADJ), (ELDRITCH)
#endif

#ifdef JP
#define MES_WALK_NEVER_MOVE "動けない。"
#define MES_WALK_NEVER_SWIM "泳げない。"
#define MES_WALK_NEVER_LAND "陸に上がれない。"
#define MES_WALK_STEED_FEAR(STEED) "%sが恐怖していて制御できない。", (STEED)
#define MES_WALK_MUST_FLY(FEATURE) "空を飛ばないと%sの上には行けない。", (FEATURE)
#else
#define MES_WALK_NEVER_MOVE "Can't move."
#define MES_WALK_NEVER_SWIM "Can't swim"
#define MES_WALK_NEVER_LAND "Can't land"
#define MES_WALK_STEED_FEAR(STEED) "%^s is too scared to control.", (STEED)
#define MES_WALK_MUST_FLY(FEATURE) "You need to fly to go through the %s.", (FEATURE)
#endif

#ifdef JP
#define MES_OBJECT_WHICH_USE "どれを使いますか？"
#define MES_OBJECT_NO_USE "使えるものがありません。"
#define MES_OBJECT_WHICH_OBSERVE "どのアイテムを調べますか? "
#define MES_OBJECT_NO_OBSERVE "調べられるアイテムがない。"
#define MES_OBJECT_WHICH_INDEN "どのアイテムを鑑定しますか? "
#define MES_OBJECT_WHICH_ALL_INDEN "すべて鑑定済みです。 "
#define MES_OBJECT_NO_INDEN "鑑定するべきアイテムがない。"
#define MES_OBJECT_WHICH_INDEN_FULLY "どのアイテムを*鑑定*しますか? "
#define MES_OBJECT_WHICH_ALL_INDEN_FULLY "すべて*鑑定*済みです。 "
#define MES_OBJECT_NO_INDEN_FULLY "*鑑定*するべきアイテムがない。"
#define MES_OBJECT_NO_INDENTIFED "このアイテムについて特に知っていることはない。"
#define MES_OBJECT_EXAMING(OBJECT) "%sを調べている..." ,(OBJECT)
#define MES_OBJECT_NO_SPECIAL "特に変わったところはないようだ。"
#else
#define MES_OBJECT_WHICH_USE "Use which item? "
#define MES_OBJECT_NO_USE "You have nothing to use." 
#define MES_OBJECT_WHICH_OBSERVE "Examine which item? "
#define MES_OBJECT_NO_OBSERVE "You have nothing to examine."
#define MES_OBJECT_WHICH_INDEN "Identify which item? "
#define MES_OBJECT_WHICH_ALL_INDEN "All items are identified. "
#define MES_OBJECT_NO_INDEN "You have nothing to identify."
#define MES_OBJECT_WHICH_INDEN_FULLY "*Identify* which item? "
#define MES_OBJECT_WHICH_ALL_INDEN_FULLY "All items are *identified*. "
#define MES_OBJECT_NO_INDEN_FULLY "You have nothing to *identify*."
#define MES_OBJECT_NO_INDENTIFED "You have no special knowledge about that item."
#define MES_OBJECT_EXAMING(OBJECT) "Examining %s...", (OBJECT)
#define MES_OBJECT_NO_SPECIAL "You see nothing special."
#endif

#ifdef JP
#define MES_OBJECT_DESTROY(OBJECT) "%sを壊した。", (OBJECT)
#define MES_OBJECT_CANNOT_DESTROY(OBJECT) "%sは破壊不可能だ。", (OBJECT)
#define MES_OBJECT_WHICH_DESTROY "どのアイテムを壊しますか? "
#define MES_OBJECT_NO_DESTROY "壊せるアイテムを持っていない。"
#define MES_OBJECT_DESTROY_VERIFY(OBJECT) "本当に%sを壊しますか? [y/n/Auto]", (OBJECT)
#else
#define MES_OBJECT_DESTROY(OBJECT) "You destroy %s.", (OBJECT)
#define MES_OBJECT_CANNOT_DESTROY(OBJECT) "You cannot destroy %s.", (OBJECT)
#define MES_OBJECT_WHICH_DESTROY "Destroy which item? "
#define MES_OBJECT_NO_DESTROY "You have nothing to destroy."
#define MES_OBJECT_DESTROY_VERIFY(OBJECT) "Really destroy %s? [y/n/Auto]", (OBJECT)
#endif

#ifdef JP
#define MES_OBJECT_WHICH_INSCRIBE "どのアイテムに銘を刻みますか? "
#define MES_OBJECT_NO_INSCRIBE "銘を刻めるアイテムがない。"
#define MES_OBJECT_WHICH_UNINSCRIBE "どのアイテムの銘を消しますか? "
#define MES_OBJECT_NO_UNINSCRIBE "銘を消せるアイテムがない。"
#define MES_OBJECT_INSCRIBING(OBJECT) "%sに銘を刻む。", (OBJECT)
#define MES_OBJECT_NO_INSCRIPTION "このアイテムには消すべき銘がない。"
#define MES_OBJECT_REMOVE_INSCRIPTION "銘を消した。"
#define MES_OBJECT_INSCRIBING_PROMPT "銘"
#else
#define MES_OBJECT_WHICH_INSCRIBE "Inscribe which item? "
#define MES_OBJECT_NO_INSCRIBE "You have nothing to inscribe."
#define MES_OBJECT_WHICH_UNINSCRIBE "Un-inscribe which item? "
#define MES_OBJECT_NO_UNINSCRIBE "You have nothing to un-inscribe."
#define MES_OBJECT_INSCRIBING(OBJECT) "Inscribing %s.", (OBJECT)
#define MES_OBJECT_NO_INSCRIPTION "That item had no inscription to remove."
#define MES_OBJECT_REMOVE_INSCRIPTION "Inscription removed."
#define MES_OBJECT_INSCRIBING_PROMPT "Inscription"
#endif



#ifdef JP
#define MES_BOUNTY_DETAIL "死体を持ち帰れば報酬を差し上げます。"
#define MES_BOUNTY_TODAY "本日の賞金首"
#define MES_BOUNTY_CURRENT "現在の賞金首"
#define MES_BOUNTY_DONE "(済)"
#else
#define MES_BOUNTY_DETAIL "Offer a prize when you bring a wanted creature's corpse"
#define MES_BOUNTY_TODAY "Wanted creature that changes from day to day"
#define MES_BOUNTY_CURRENT "Wanted creatures"
#define MES_BOUNTY_DONE "(done)"
#endif

#ifdef JP
#define MES_EQUIP_HEAVY_BOW "こんな重い弓を装備しているのは大変だ。"
#define MES_EQUIP_FITTING_BOW "この弓なら装備していても辛くない。"
#define MES_EQUIP_RELIEVE_HEAVY_BOW "重い弓を装備からはずして体が楽になった。"
#define MES_EQUIP_HEAVY_WEAPON "こんな重い武器を装備しているのは大変だ。"
#define MES_EQUIP_FITTING_WEAPON "これなら装備していても辛くない。"
#define MES_EQUIP_HEAVY_WEAPON_STILL "まだ武器が重い。"
#define MES_EQUIP_RELIEVE_HEAVY_WEAPON "重い武器を装備からはずして体が楽になった。"
#define MES_EQUIP_NO_RIDING_WEAPON "この武器は乗馬中に使うにはむかないようだ。"
#define MES_EQUIP_SUITABLE_WALKING_WEAPON "この武器は徒歩で使いやすい。"
#define MES_EQUIP_RIDING_WEAPON "これなら乗馬中にぴったりだ。"
#define MES_EQUIP_HEAVY_ARMOR "装備が重くてバランスを取れない。"
#define MES_EQUIP_LIGHT_ARMOR "バランスがとれるようになった。"
#define MES_EQUIP_UNCONTROL_STEED "馬を操れない。"
#define MES_EQUIP_CONTROL_STEED "馬を操れるようになった。"
#else
#define MES_EQUIP_HEAVY_BOW "You have trouble wielding such a heavy bow."
#define MES_EQUIP_FITTING_BOW "You have no trouble wielding your bow."
#define MES_EQUIP_RELIEVE_HEAVY_BOW "You feel relieved to put down your heavy bow."
#define MES_EQUIP_HEAVY_WEAPON "You have trouble wielding such a heavy weapon."
#define MES_EQUIP_FITTING_WEAPON "You have no trouble wielding your weapon."
#define MES_EQUIP_HEAVY_WEAPON_STILL "You have still trouble wielding a heavy weapon."
#define MES_EQUIP_RELIEVE_HEAVY_WEAPON "You feel relieved to put down your heavy weapon."
#define MES_EQUIP_NO_RIDING_WEAPON "This weapon is not suitable for use while riding."
#define MES_EQUIP_SUITABLE_WALKING_WEAPON "This weapon was not suitable for use while riding."
#define MES_EQUIP_RIDING_WEAPON "This weapon is suitable for use while riding."
#define MES_EQUIP_HEAVY_ARMOR "The weight of your armor disrupts your balance."
#define MES_EQUIP_LIGHT_ARMOR "You regain your balance."
#define MES_EQUIP_UNCONTROL_STEED "You are using both hand for fighting, and you can't control a riding pet."
#define MES_EQUIP_CONTROL_STEED "You began to control riding pet with one hand."
#endif

#ifdef JP
#define MES_DAMAGE_RES1 "にはかなりの耐性がある。"
#define MES_DAMAGE_RES2 "には耐性がある。"
#define MES_DAMAGE_RES3 "にはわずかに耐性がある。"
#define MES_DAMAGE_RES4 "はひどい痛手を受けた。"
#define MES_DAMAGE_HURT_LITE "光で肉体が焦がされた！"
#define MES_DAMAGE_RES_TIME(CREATURE) "%sは時間が通り過ぎていく様子を感じた。", (CREATURE)
#define MES_DAMAGE_VOID_INVULN "バリアを切り裂いた！"
#else
#define MES_DAMAGE_RES1 " resists strongly."
#define MES_DAMAGE_RES2 " resists."
#define MES_DAMAGE_RES3 " resists a little."
#define MES_DAMAGE_RES4 " is hit hard."
#define MES_DAMAGE_HURT_LITE "The light scorches your flesh!"
#define MES_DAMAGE_RES_TIME(CREATURE) "%s felt as if time is passing %s by.", (CREATURE), (CREATURE)
#define MES_DAMAGE_VOID_INVULN "The barrier is penetrated!"
#endif

#ifdef JP
#define MES_DISARM_NO_TRAP "トラップが見あたらない。"
#define MES_DISARM_NO_TRAP_CHEST "箱にはトラップが仕掛けられていない。"
#define MES_DISARM_FAILED(TARGET) "%sの解除に失敗した。", (TARGET)
#define MES_DISARM_FUMBLE "トラップを作動させてしまった！"
#else
#define MES_DISARM_NO_TRAP "You don't see any traps."
#define MES_DISARM_NO_TRAP_CHEST "The chest is not trapped."
#define MES_DISARM_FAILED(TARGET) "You failed to disarm the %s.", (TARGET)
#define MES_DISARM_FUMBLE "You set off a trap!"
#endif

#ifdef JP
#define MES_TRAVEL_ALREADY "すでにそこにいます。"
#define MES_TRAVEL_CANNOT "そこには行くことができません。"
#else
#define MES_TRAVEL_ALREADY "You are already there."
#define MES_TRAVEL_CANNOT "You cannot travel there!"
#endif

#ifdef JP
#define MES_BASH_NO_TARGET "そこには体当たりするものが見当たらない。"
#define MES_BASH_DO(TARGET) "%sに体当たりをした！", (TARGET)
#define MES_BASH_CRUSHED(TARGET) "%sを壊した！", (TARGET)
#define MES_BASH_HOLD(TARGET) "この%sは頑丈だ。", (TARGET)
#define MES_BASH_OFF_BALANCE "体のバランスをくずしてしまった。"
#else
#define MES_BASH_NO_TARGET "You see nothing there to bash."
#define MES_BASH_DO(TARGET) "You smash into the %s!", (TARGET)
#define MES_BASH_CRUSHED(TARGET) "The %s crashes open!", (TARGET)
#define MES_BASH_HOLD(TARGET) "The %s holds firm.", (TARGET)
#define MES_BASH_OFF_BALANCE "You are off-balance."
#endif

#ifdef JP
#define MES_SPIKE_NO_TARGET "そこにはくさびを打てるものが見当たらない。"
#define MES_SPIKE_NO_SPIKE "くさびを持っていない。"
#define MES_SPIKE_JAM(TARGET) "%sにくさびを打ち込んだ。", (TARGET)
#else
#define MES_SPIKE_NO_TARGET "You see nothing there to spike."
#define MES_SPIKE_NO_SPIKE "You have no spikes!"
#define MES_SPIKE_JAM(TARGET) "You jam the %s with a spike.", (TARGET)
#endif

#ifdef JP
#define MES_TIME_STRANGE "変な時刻だ。"
#define MES_TIME_DETAIL(DAY, HH, MM) "%s日目, 時刻は%d:%02d %sです。", DAY, (HH % 12 == 0) ? 12 : (HH % 12), MM, (HH < 12) ? "AM" : "PM"
#else
#define MES_TIME_STRANGE "It is a strange time."
#define MES_TIME_DETAIL(DAY, HH, MM) "This is day %s. The time is %d:%02d %s.", DAY, (HH % 12 == 0) ? 12 : (HH % 12), MM, (HH < 12) ? "AM" : "PM"
#endif

#ifdef JP
#define MES_KNOW_PET "現在のペット"
#define MES_KNOW_ALIVE_UNIQUES "まだ生きているユニーク・クリーチャー"
#define MES_KNOW_NO_ALIVE_UNIQUES "既知の生存ユニークはいません。\n"
#define MES_KNOW_NO_KILLED "あなたはまだ誰も殺していない。\n\n"
#define MES_KNOW_KILLED(NUMBER) "あなたは%ld体のクリーチャーを殺している。\n\n", (NUMBER)
#else
#define MES_KNOW_PET "Current Pets"
#define MES_KNOW_ALIVE_UNIQUES "Alive Uniques"
#define MES_KNOW_NO_ALIVE_UNIQUES "No known uniques alive.\n"
#define MES_KNOW_NO_KILLED "You have killed no creatures yet.\n\n"
#define MES_KNOW_KILLED(NUMBER) "You have killed %ld %s.\n\n", (NUMBER), ((NUMBER) == 1) ? "creature" : "creatures"
#endif

#ifdef JP
#define MES_RUSH_NO_ENTER "ここには入身では入れない。"
#define MES_RUSH_DONE(TARGET) "素早く%sの懐に入り込んだ！", (TARGET)
#else
#define MES_RUSH_NO_ENTER "You can't move to that place."
#define MES_RUSH_DONE(TARGET) "You quickly jump in and attack %s!", (TARGET)
#endif

#ifdef JP
#define MES_EARTHQUAKE_DONE1 "ダンジョンの壁が崩れた！"
#define MES_EARTHQUAKE_DONE2 "ダンジョンの床が不自然にねじ曲がった！"
#define MES_EARTHQUAKE_DONE3 "ダンジョンが揺れた！崩れた岩が頭に降ってきた！"
#define MES_EARTHQUAKE_HEAVY_CRUSHED "あなたはひどい怪我を負った！"
#define MES_EARTHQUAKE_EVADE "降り注ぐ岩をうまく避けた！"
#define MES_EARTHQUAKE_CRUSHED1 "岩石があなたに直撃した!"
#define MES_EARTHQUAKE_CRUSHED2 "あなたは床と壁との間に挟まれてしまった！"
#else
#define MES_EARTHQUAKE_DONE1 "The cave ceiling collapses!"
#define MES_EARTHQUAKE_DONE2 "The cave floor twists in an unnatural way!"
#define MES_EARTHQUAKE_DONE3 "The cave quakes!  You are pummeled with debris!"
#define MES_EARTHQUAKE_HEAVY_CRUSHED "You are severely crushed!"
#define MES_EARTHQUAKE_EVADE "You nimbly dodge the blast!"
#define MES_EARTHQUAKE_CRUSHED1 "You are bashed by rubble!"
#define MES_EARTHQUAKE_CRUSHED2 "You are crushed between the floor and ceiling!"
#endif

#ifdef JP
#define MES_DIS_MINION_ASK "本当に全ペットを爆破しますか？"
#define MES_DIS_MINION_CANCEL(MINION) "%sは爆破されるのを嫌がり、勝手に自分の世界へと帰った。", (MINION)
#else
#define MES_DIS_MINION_ASK "You will blast all pets. Are you sure? "
#define MES_DIS_MINION_CANCEL(MINION)  "%^s resists to be blasted, and run away.", (MINION)
#endif

#ifdef JP
#define MES_PATRON_MUTATION(PATRON) "%^sは褒美としてあなたを突然変異させた。"
#define MES_PATRON_POLY_SELF "「汝、新たなる姿を必要とせり！」"
#define MES_PATRON_GAIN_EXP "「汝は良く行いたり！続けよ！」"
#define MES_PATRON_LOSE_EXP "「下僕よ、汝それに値せず。」"
#define MES_PATRON_GOOD_OBJ "「我が与えし物を賢明に使うべし。」"
#define MES_PATRON_GOOD_WEP "「汝の行いは貴き剣に値せり。」"
#define MES_PATRON_GOOD_OBS "「汝の行いは貴き報いに値せり」"
#define MES_PATRON_GREAT_OBS "「下僕よ、汝の献身への我が惜しみ無き報いを見るがよい。」"
#define MES_PATRON_TY_CURSE "「下僕よ、汝傲慢なり。」"
#define MES_PATRON_S_ENEMY "「我が下僕たちよ、かの傲慢なる者を倒すべし！」"
#define MES_PATRON_H_SUMMON "「汝、より強き敵を必要とせり！」"
#define MES_PATRON_HAVOC "「死と破壊こそ我が喜びなり！」"
#define MES_PATRON_GAIN_ABL "「留まるのだ、下僕よ。余が汝の肉体を鍛えん。」"
#define MES_PATRON_LOSE_ABL "「下僕よ、余は汝に飽みたり。」"
#define MES_PATRON_RUIN_ABL "「汝、謙虚たることを学ぶべし！」"
#define MES_PATRON_POLY_WND(PATRON) "%sの力が触れるのを感じた。", (PATRON)
#define MES_PATRON_AUGM_ABL "「我がささやかなる賜物を受けとるがよい！」"
#define MES_PATRON_HURT_LOT "「苦しむがよい、無能な愚か者よ！」"
#define MES_PATRON_HEAL_FUL "「甦るがよい、我が下僕よ！」"
#define MES_PATRON_CURSE_WEAPON "「汝、武器に頼ることなかれ。」"
#define MES_PATRON_CURSE_ARMOR "「汝、防具に頼ることなかれ。」"
#define MES_PATRON_PISS_OFF "「我を怒りしめた罪を償うべし。」"
#define MES_PATRON_GENOCIDE "「我、汝の敵を抹殺せん！」"
#define MES_PATRON_ASSULT(PATRON) "%sの力が敵を攻撃するのを感じた！", (PATRON)
#else
#define MES_PATRON_MUTATION(PATRON) "%^s rewards you with a mutation!"
#define MES_PATRON_POLY_SELF "'Thou needst a new form!'"
#define MES_PATRON_GAIN_EXP "'Well done, Lead on!'"
#define MES_PATRON_LOSE_EXP "'Thou didst not deserve that, slave.'"
#define MES_PATRON_GOOD_OBJ "'Use my gift wisely.'"
#define MES_PATRON_GOOD_WEP "'Thy deed hath earned thee a worthy blade.'"
#define MES_PATRON_GOOD_OBS "'Thy deed hath earned thee a worthy reward.'"
#define MES_PATRON_GREAT_OBS "'Behold, mortal, how generously I reward thy loyalty.'"
#define MES_PATRON_TY_CURSE "'Thou art growing arrogant, mortal.'"
#define MES_PATRON_S_ENEMY "'My pets, destroy the arrogant mortal!'"
#define MES_PATRON_H_SUMMON "'Thou needst worthier opponents!'"
#define MES_PATRON_HAVOC "'Death and destruction! This pleaseth me!'"
#define MES_PATRON_GAIN_ABL "'Stay, mortal, and let me mold thee.'"
#define MES_PATRON_LOSE_ABL "'I grow tired of thee, mortal.'"
#define MES_PATRON_RUIN_ABL "'Thou needst a lesson in humility, mortal!'"
#define MES_PATRON_POLY_WND(PATRON) "You feel the power of %s touch you.", (PATRON)
#define MES_PATRON_AUGM_ABL "'Receive this modest gift from me!'"
#define MES_PATRON_HURT_LOT "'Suffer, pathetic fool!'"
#define MES_PATRON_HEAL_FUL "'Rise, my servant!'"
#define MES_PATRON_CURSE_WEAPON "'Thou reliest too much on thy weapon.'"
#define MES_PATRON_CURSE_ARMOR "'Thou reliest too much on thine equipment.'"
#define MES_PATRON_PISS_OFF "'Now thou shalt pay for annoying me.'"
#define MES_PATRON_GENOCIDE "'Let me relieve thee of thine oppressors!'"
#define MES_PATRON_ASSULT(PATRON) "You can feel the power of %s assault your enemies!", (PATRON)
#endif

#ifdef JP
#define MES_OPTION_WARN_CHEAT "詐欺師は決して勝利できない。"
#define MES_OPTION_UNUSED "(未使用)"
#define MES_OPTION_MAIN_TITLE "[ オプションの設定 ]"
#define MES_OPTION_CMD_HP_WARNING "コマンド: 低ヒットポイント警告"
#define MES_OPTION_CMD_MP_CHECK "コマンド: 低魔力色閾値"
#else
#define MES_OPTION_WARN_CHEAT "Cheaters never win."
#define MES_OPTION_UNUSED "(Unused option)"
#define MES_OPTION_MAIN_TITLE [ Game Options ]"
#define MES_OPTION_CMD_HP_WARNING "Command: Hitpoint Warning"
#define MES_OPTION_CMD_MP_CHECK "Command: Mana Color Threshold"
#endif

#ifdef JP
#define MES_STATUS_SELECTION "['c'で名前変更, 'f'でファイルへ書出, 'h'でモード変更, ESCで終了]"
#else
#define MES_STATUS_SELECTION "['c' to change name, 'f' to file, 'h' to change mode, or ESC]"
#endif

#ifdef JP
#define MES_DIARY_ASK_ERASE "本当に記録を消去しますか？"
#define MES_DIARY_DELETED "記録を消去しました。"
#define MES_DIARY_DELETE_FAIL(FILE) "%s の消去に失敗しました。", (FILE)
#else
#define MES_DIARY_ASK_ERASE "Do you really want to delete all your record? "
#define MES_DIARY_DELETED "deleted record."
#define MES_DIARY_DELETE_FAIL(FILE) "failed to delete %s.", (FILE)
#endif

#ifdef JP
#define MES_MUTATION_SOMETHING "何か変わった気がする！"
#else
#define MES_MUTATION_SOMETHING "You feel different!"
#endif

#ifdef JP
#define MES_CAST_NONE "呪文を唱えられない！"
#define MES_CAST_NO_NEED_LEARN "呪文を学習する必要はない！"
#else
#define MES_CAST_NONE "You cannot cast spells!"
#define MES_CAST_NO_NEED_LEARN "You don't have to learn spells!"
#endif
