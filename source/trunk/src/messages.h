
#ifdef JP
#define BRACKET_ARTIFACT "『%s』"
#define BRACKET_DAMMY_ARTIFACT "《%s》"
#else
#define BRACKET_ARTIFACT "'%s'"
#define BRACKET_DAMMY_ARTIFACT "'%s'"
#endif

#ifdef JP
#define BRACK_D_ANGLE "《"
#define KET_D_ANGLE "》"
#else
#define BRACK_D_ANGLE "<<"
#define KET_D_ANGLE ">>"
#endif

// Keywords

#ifdef JP
#define KEYWORD_CURSED "呪われている"
#else
#define KEYWORD_CURSED "cursed"
#endif

#ifdef JP
#define MES_INITIALIZE_USER "[ユーザー設定ファイルを初期化しています]"
#define MES_INITIALIZE_COMPLETE "[初期化終了]"
#else
#define MES_INITIALIZE_USER "[Initializing user pref files]"
#define MES_INITIALIZE_COMPLETE "[Initialization complete]"
#endif

#ifdef JP

#define MES_SYS_YOU_DIED(CREATURE) (has_trait((CREATURE), TRAIT_ANDROID) ? "あなたは壊れました" : "あなたは死にました")
#define MES_SYS_MORE "-- 続く --"
#define MES_SYS_WIZARD_MODE_ON "ウィザードモード突入。"
#define MES_SYS_WIZARD_MODE_OFF "ウィザードモード解除。"
#define MES_SYS_STARTING_GAME "ゲームを開始します。お待ち下さい..."
#define MES_SYS_PLEASE_WAIT "お待ち下さい..."
#define MES_SYS_FAILED_TEMPFILE "一時ファイル %s を作成できませんでした。"
#define MES_SYS_FAILED_FILEOPEN "ファイルを開けません。"
#define MES_SYS_FAILED_FILEOPEN2 "%s を開くことができませんでした。"
#define MES_SYS_FAILED_FILEREAD "ファイルを読めません。"
#define MES_SYS_REPLACE_FILE "現存するファイルに上書きしますか? (%s)"
#define MES_SYS_HIT_ANY_KEY "何かキーを押して下さい。"
#define MES_SYS_HIT_SPACE_KEY "スペースキーで続行"
#define MES_SYS_HIT_VERIFY_KEY "確認のため '@' を押して下さい。"
#define MES_SYS_PLACE_SPACE "場所を選んでスペースキーを押して下さい。"
#define MES_SYS_OPTION_AUX "初期オプション((*)はスコアに影響)"
#define MES_SYS_OPTION_AUX2 "初期オプション(参照のみ)"
#define MES_SYS_AUTO_SAVING "自動セーブ中"
#define MES_SYS_GAME_SAVING "ゲームをセーブしています..."
#define MES_SYS_QUITING_KEY "リターンキーか ESC キーを押して下さい。"
#define MES_SYS_ANY_KEY "[何かキーを押すとゲームに戻ります]"
#define MES_SYS_SAVEFILE_ERROR(NUM, MAJ, MIN, EXT) "エラー(%s)がバージョン%d.%d.%d 用セーブファイル読み込中に発生。",(NUM), (MAJ), (MIN), (EXT)
#define MES_SYS_SAVEFILE_ERROR2 "セーブファイルを解析出来ません。"
#define MES_SYS_SAVEFILE_ERROR3 "セーブファイルが壊れています"
#define MES_SYS_SAVEFILE_ERROR4(ERROR_NUM) "セーブファイルが壊れています: ERR:%d", (ERROR_NUM)
#define MES_SYS_SAVEFILE_CONVERTED(MAJ, MIN, EXT) "バージョン %d.%d.%d 用のセーブ・ファイルを変換しました。", (MAJ), (MIN), (EXT)
#define MES_SYS_SAVE_FAILED "セーブ失敗！"
#define MES_SYS_SCORE_FAILED "スコア・ファイルが使用できません。"
#define MES_SYS_PANIC_SAVE_SUCCEED "緊急セーブ成功！"
#define MES_SYS_PANIC_SAVE_FAILED "緊急セーブ失敗！"
#define MES_SYS_TRAITS(CREATURE) "%sには以下の特性がある", (CREATURE)->name
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
#define MES_SYS_HERE "真上"
#define MES_SYS_NORTH "北"
#define MES_SYS_SOUTH "南"
#define MES_SYS_EAST "東"
#define MES_SYS_WEST "西"
#define MES_SYS_MONEY "所持金"
#define MES_SYS_PRICE "価格"
#define MES_SYS_WEIGHT "重量"

#define MES_SYS_MODIFICATION "能力修正"

#define MES_SYS_VISUAL_COMMAND "コマンド (n/N/^N/a/A/^A/c/C/^C/v/V/^V): "
#define MES_SYS_ASK_USE "%sを使いますか？"
#define MES_SYS_ASK_SURE "よろしいですか？"

#define MES_SYS_CHARGE_NUM(NUM) "回分"

#define MES_SYS_SKILL "技能値"

#define MES_SYS_AUTODUMP_WARNING1 "# *警告!!* 以降の行は自動生成されたものです。\n"
#define MES_SYS_AUTODUMP_WARNING2 "# *警告!!* 後で自動的に削除されるので編集しないでください。\n"
#define MES_SYS_AUTODUMP_WARNING3 "# *警告!!* 以上の行は自動生成されたものです。\n"

#define MES_SYS_NO_TOWN_ITEM "町のアイテムがない！"
#define MES_SYS_NO_VALUT_TRAP "地下室のトラップを配置できません"

#define MES_SYS_ASK_SCREEN_DUMP "記念撮影しますか？ [(y)es/(h)tml/(n)o] "
#define MES_SYS_SCREEN_DUMPED "画面(記念撮影)をファイルに書き出しました。"
#define MES_SYS_SCREEN_LOADED "ファイルに書き出された画面(記念撮影)をロードしました。"
#define MES_SYS_HP_WARNING "*** 警告:低ヒット・ポイント！ ***"
#define MES_SYS_FIND "検索: "

#else

#define MES_SYS_YOU_DIED(CREATURE) (has_trait((CREATURE), TRAIT_ANDROID) ? "You were broken." : "You died.")
#define MES_SYS_MORE "-- MORE --"
#define MES_SYS_WIZARD_MODE_ON "Wizard mode on."
#define MES_SYS_WIZARD_MODE_OFF "Wizard mode off."
#define MES_SYS_STARTING_GAME "Starting Game, Please wait..."
#define MES_SYS_PLEASE_WAIT "Please wait..."
#define MES_SYS_FAILED_TEMPFILE "Failed to create temporary file %s."
#define MES_SYS_FAILED_FILEOPEN "Can not open file."
#define MES_SYS_FAILED_FILEOPEN2 "Failed to open %s."
#define MES_SYS_FAILED_FILEREAD "Cannot read file."
#define MES_SYS_REPLACE_FILE "Replace existing file %s? "
#define MES_SYS_HIT_ANY_KEY "Hit Any Key。"
#define MES_SYS_HIT_VERIFY_KEY "Please verify by typing the '@' sign: "
#define MES_SYS_PLACE_SPACE "Select a point and press space."

#define MES_SYS_OPTION_AUX "Birth Option((*)s effect score)"
#define MES_SYS_OPTION_AUX2 "Birth Options(browse only)"
#define MES_SYS_AUTO_SAVING "Autosaving the game..."
#define MES_SYS_GAME_SAVING "Saving game..."
#define MES_SYS_QUITING_KEY "Press Return (or Escape)."
#define MES_SYS_ANY_KEY "[Press any key to continue]"
#define MES_SYS_HIT_SPACE_KEY "Press the space bar to continue"
#define MES_SYS_SAVEFILE_ERROR(NUM, MAJ, MIN, EXT) "Error (%s) reading %d.%d.%d savefile.", (NUM), (MAJ), (MIN), (EXT)
#define MES_SYS_SAVEFILE_ERROR2 "Cannot parse savefile."
#define MES_SYS_SAVEFILE_ERROR3 "Broken savefile"
#define MES_SYS_SAVEFILE_ERROR4(ERROR_NUM) "broken savefile: ERR:%d", (ERROR_NUM)
#define MES_SYS_SAVEFILE_CONVERTED(MAJ, MIN, EXT) "Converted a %d.%d.%d savefile.", (MAJ), (MIN), (EXT)
#define MES_SYS_SAVE_FAILED "death save failed!"
#define MES_SYS_SCORE_FAILED "Score file unavailable."
#define MES_SYS_PANIC_SAVE_SUCCEED "Panic save succeeded!"
#define MES_SYS_PANIC_SAVE_FAILED "panic save failed!"
//TODO
#define MES_SYS_TRAITS(CREATURE) "%s have these trait...", (CREATURE)->name
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
#define MES_SYS_HERE ""
#define MES_SYS_NORTH " North"
#define MES_SYS_SOUTH " South"
#define MES_SYS_EAST " East"
#define MES_SYS_WEST " West"
#define MES_SYS_MONEY "AU"
#define MES_SYS_PRICE "Price"
#define MES_SYS_WEIGHT "Weight"

#define MES_SYS_MODIFICATION "Modification"

#define MES_SYS_VISUAL_COMMAND "Command (n/N/^N/a/A/^A/c/C/^C/v/V/^V):"
#define MES_SYS_ASK_USE "use %s?"
#define MES_SYS_ASK_SURE "Are you sure?"

#define MES_SYS_CHARGE_NUM(NUM) ((NUM) > 1 ? " charges" : "charge")

#define MES_SYS_SKILL "Proficiency"

#define MES_SYS_AUTODUMP_WARNING1 "# *Warning!*  The lines below are an automatic dump.\n"
#define MES_SYS_AUTODUMP_WARNING2 "# Don't edit them; changes will be deleted and replaced automatically.\n"
#define MES_SYS_AUTODUMP_WARNING3 "# *Warning!*  The lines above are an automatic dump.\n"

#define MES_SYS_NO_TOWN_ITEM "No town objects!"
#define MES_SYS_NO_VALUT_TRAP "Could not place vault trap."

#define MES_SYS_ASK_SCREEN_DUMP "Save screen dump? [(y)es/(h)tml/(n)o] "
#define MES_SYS_SCREEN_DUMPED "Screen dump saved."
#define MES_SYS_SCREEN_LOADED "Screen dump loaded."
#define MES_SYS_HP_WARNING "*** LOW HITPOINT WARNING! ***"
#define MES_SYS_FIND "Find: "

#endif

#if JP
#define MES_SYS_WIN_CANNOT_SAVE "今はセーブすることは出来ません。"
#define MES_SYS_WIN_CANNOT_EXIT "今は終了できません。"
#define MES_SYS_WIN_INITIALIZE_NOW "まだ初期化中です..."
#define MES_SYS_WIN_CANNOT_OPEN_MOVIE "プレイ中はムービーをロードすることができません！"
#define MES_SYS_WIN_MAIN_MUST_ENABLE "メインウィンドウは非表示にできません！"
#else
#define MES_SYS_WIN_CANNOT_SAVE "You may not do that right now."
#define MES_SYS_WIN_CANNOT_EXIT "You may not do that right now."
#define MES_SYS_WIN_INITIALIZE_NOW "You cannot do that yet..."
#define MES_SYS_WIN_CANNOT_OPEN_MOVIE "You can't open a movie while you're playing!"
#define MES_SYS_WIN_MAIN_MUST_ENABLE "You are not allowed to do that!"
#endif

#ifdef JP
#define MES_LOAD_START(VER1, VER2, VER3) "バージョン %d.%d.%d のセーブ・ファイルをロード中...", (VER1), (VER2), (VER3)
#define MES_LOAD_RANDOM_SEED "乱数情報をロードしました"
#define MES_LOAD_OPTION "オプションをロードしました"
#define MES_LOAD_MESSAGE "メッセージをロードしました"
#define MES_LOAD_WORLD "世界情報をロードしました"
#define MES_LOAD_ERROR_WILDERNESS(SAVE_X, MAX_X, SAVE_Y, MAX_Y) "荒野が大きすぎる(X:%u/%u) (Y:%u/%u)", (SAVE_X), (MAX_X), (SAVE_Y), (MAX_Y)
#define MES_LOAD_FLOOR(NUM) "ダンジョン復元中...フロア数:%d", (NUM)
#else
#define MES_LOAD_START(VER1, VER2, VER3) "Loading a %d.%d.%d savefile...", (VER1), (VER2), (VER3)
#define MES_LOAD_RANDOM_SEED "Loaded Randomizer Info"
#define MES_LOAD_OPTION "Loaded Option Flags"
#define MES_LOAD_MESSAGE "Loaded Messages"
#define MES_LOAD_WORLD "Loaded world information"
#define MES_LOAD_ERROR_WILDERNESS(SAVE_X, MAX_X, SAVE_Y, MAX_Y) "Wilderness is too big (X:%u/%u) (Y:%u/%u)", (SAVE_X), (MAX_X), (SAVE_Y), (MAX_Y)
#define MES_LOAD_FLOOR(NUM) "Restoring Dungeon... NUM:%d", (NUM)
#endif

#if JP
#define KW_MORE "-続く-"
#define KW_CREATURE "クリーチャー"
#define KW_NAME "名前"
#define KW_RACE "種族"
#define KW_PURE_BLOOD "純血種"
#define KW_CLASS "職業"
#define KW_REALM "魔法"
#define KW_CHARA "性格"
#define KW_PATRON "主神"
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
#define KW_KARMA "業"
#define KW_ALIGNMENT "属性"
#define KW_PROFICIENCY "技能値"
#define KW_DURING "期間"
#define KW_RANGE "範囲"
#define KW_HEAL "回復"
#define KW_FAIL "失率"
#define KW_STRANGE "未知"
#define KW_FORGOTTEN "忘却"
#define KW_ILLEGIBLE "判読不能"
#define KW_UNTRIED "未経験"
#define KW_INVENTORY "所持品"
#define KW_EQUIPMENT "装備品"
#define KW_CARRYING_WEIGHT "所持重量"
#define KW_EQUIPMENT_WEIGHT "装備重量"
#define KW_STAFF "杖"
#define KW_WAND "魔法棒"
#define KW_ROD "ロッド"
#define KW_YOU "あなた"
#define KW_CREATURE_PROFILE "クリーチャーのプロフィール"
#define KW_CREATURE_PROFILE_EDITING "クリーチャーのプロフィール - 編集中"
#define KW_CREATURE_PROFILE_EDITED "クリーチャーのプロフィール - 編集済み"
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
#define KW_SURE "決定"
#define KW_CANCEL "キャンセル"
#define KW_FAILED "失敗"
#define KW_NONE "なし"
#define KW_NOTHING "なし"
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
#define KW_OFFER "提示金額"
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
#define KW_HISSATSU "必殺技"
#define KW_SONG "歌"
#define KW_MIRACLE "奇跡"
#define KW_SPELL "魔法"
#define KW_STUDY "学ぶ"
#define KW_SOMETHING_STRANGE "何か奇妙な物"
#define KW_NOTHING_TARGET "指定なし"
#define KW_UNEXPLORED_ZONE "未踏破地帯"
#define KW_ROAD "道"
#define KW_MESSAGE "メッセージ"
#define KW_WINNING_MESSAGE "*勝利*メッセージ"
#define KW_LEAVE "放置"
#define KW_DESTROY "破壊"
#define KW_PICKUP "拾う"
#define KW_QUERY "確認"
#define KW_TRY "本当に"
#define KW_FEATURE "地形"
#define KW_CAMOUFLAGE "偽装地形"
#define KW_QUANTITY "個数"
#define KW_KILLS "殺害数"
#define KW_SYM "文字"
#define KW_PET "ペット"
#define KW_FRIEND "友軍"
#define KW_CLONE "クローン"
#define KW_WANTED "賞金首"
#define KW_CHARGING "(充填中)"
#define KW_FLOORITEM "床上"
#define KW_WITH_THOHANDED "(武器を両手持ち)"
#define KW_CHARA_INVENTORY "キャラクタの持ち物"
#define KW_CHARA_EQUIPMENT "キャラクタの装備"
#define KW_MUSIUM_LIST "博物館のアイテム"
#define KW_JUST_LIFTING "運搬中"
#define KW_SHOOTING "射撃用"
#define KW_UNKNOWN "不明"
#define KW_BODY "体"
#define KW_WORLD_MAP "混沌の地平"
#define KW_ARENA "闘技場"
#define KW_AUTOSAVE "自動セーブ"
#else
#define KW_MORE "-more-"
#define KW_CREATURE "Creatures"
#define KW_NAME "Name"
#define KW_RACE "Race"
#define KW_PURE_BLOOD "Pure Blood"
#define KW_CLASS "Class"
#define KW_REALM "Realm"
#define KW_CHARA "Chara"
#define KW_PATRON "Patron"
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
#define KW_KARMA "Karma"
#define KW_ALIGNMENT "Alignment"
#define KW_PROFICIENCY "Proficiency"
#define KW_DURING "Dur"
#define KW_RANGE "Range"
#define KW_HEAL "Heal"
#define KW_FAIL "Fail"
#define KW_STRANGE "Strange"
#define KW_FORGOTTEN "Forgotten"
#define KW_ILLEGIBLE "Illegible"
#define KW_UNTRIED "Untried"
#define KW_INVENTORY "Inventory"
#define KW_EQUIPMENT "Equipment"
#define KW_CARRYING_WEIGHT "Carrying Weight"
#define KW_EQUIPMENT_WEIGHT "Equipment Weight"
#define KW_STAFF "staff"
#define KW_WAND "wand"
#define KW_ROD "rod"
#define KW_YOU "You"
#define KW_CREATURE_PROFILE "Creature Profile"
#define KW_CREATURE_PROFILE_EDITING "Creature Profile - Edit Mode"
#define KW_CREATURE_PROFILE_EDITED "Creature Profile - Edited"
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
#define KW_SURE "Sure"
#define KW_NOTHING "Nothing"
#define KW_CANCEL "Cancel"
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
#define KW_OFFER "Offer"
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
#define KW_HISSATSU "Art"
#define KW_SONG "Song"
#define KW_MIRACLE "Miracle"
#define KW_SPELL "Spell"
#define KW_STUDY "Study"
#define KW_SOMETHING_STRANGE "something strange"
#define KW_NOTHING_TARGET "nothing"
#define KW_UNEXPLORED_ZONE "unexplored zone"
#define KW_ROAD "road"
#define KW_MESSAGE "message"
#define KW_WINNING_MESSAGE "*Winning* message"
#define KW_LEAVE "Leave"
#define KW_DESTROY "Destroy"
#define KW_PICKUP "Pickup"
#define KW_QUERY "Query"
#define KW_TRY "Try"
#define KW_FEATURE "Feature"
#define KW_CAMOUFLAGE "Camouflage"
#define KW_QUANTITY "Quantity"
#define KW_KILLS "Kills"
#define KW_SYM "Sym"
#define KW_PET "Pet"
#define KW_FRIEND "Friend"
#define KW_CLONE "Clone"
#define KW_WANTED "Wanted"
#define KW_CHARGING "(charging)""
#define KW_FLOORITEM "On a floor"
#define KW_WITH_THOHANDED "(wielding with two-hands)"
#define KW_CHARA_INVENTORY "Character inventory"
#define KW_CHARA_EQUIPMENT "Character Equipment"
#define KW_MUSIUM_LIST "Museum"
#define KW_JUST_LIFTING "Just lifting"
#define KW_SHOOTING "Shooting"
#define KW_UNKNOWN "UNKNOWN"
#define KW_BODY "body"
#define KW_WORLD_MAP "The Surface of Chaos"
#define KW_ARENA "Arena"
#define KW_AUTOSAVE "Autosave"
#endif

#ifdef JP
#define MES_FEATURE_KILLED "まばゆい閃光が走った！"
#else
#define MES_FEATURE_KILLED "There is a bright flash of light!"
#endif

#ifdef JP
#define MES_TRAP_FOUND "トラップだ。"
#define MES_TRAP_FOUND_CHEST "箱に仕掛けられたトラップを発見した！"

#define MES_TRAP_DOOR "落とし戸に落ちた！"
#define MES_TRAP_DOOR_AVOID "落とし戸を飛び越えた。"

#define MES_TRAP_PIT "落とし穴に落ちてしまった！"
#define MES_TRAP_PIT_AVOID "落とし穴を飛び越えた。"
#define MES_TRAP_SPIKED_PIT "スパイクが敷かれた落とし穴に落ちてしまった！"
#define MES_TRAP_SPIKED_PIT_AVOID "トゲのある落とし穴を飛び越えた。"
#define MES_TRAP_INPILED "スパイクが刺さった！"
#define MES_TRAP_POISON_INPILED "毒を塗られたスパイクが刺さった！"

#define MES_TRAP_BLIND "黒いガスに包み込まれた！"
#define MES_TRAP_CONFUSE "きらめくガスに包み込まれた！"
#define MES_TRAP_POISON "刺激的な緑色のガスに包み込まれた！"
#define MES_TRAP_SLEPT "奇妙な白い霧に包まれた！"

#define MES_TRAP_NEEDLE "仕掛けられていた小さな針に刺されてしまった！"
#define MES_TRAP_DARTS "小さなダーツが飛んできて刺さった！"
#define MES_TRAP_DARTS_MISSED "小さなダーツが飛んできた！が、運良く当たらなかった。"
#define MES_TRAP_FIRE "炎に包まれた！"
#define MES_TRAP_ACID "酸が吹きかけられた！"
#define MES_TRAP_ACID_FLOW "突然床から酸が溢れ出した！"
#define MES_TRAP_POISON_FLOW "突然床から毒液溢れ出した！"
#define MES_TRAP_PIRANHAS "突然壁から水が溢れ出した！ピラニアがいる！"

#define MES_TRAP_POISONOUS_GAS "突如吹き出した緑色のガスに包み込まれた！"
#define MES_TRAP_PARALYZE_GAS "突如吹き出した黄色いガスに包み込まれた！"
#define MES_TRAP_SUMMONING "突如吹き出した煙に包み込まれた！"
#define MES_TRAP_S_ELEMENTAL "宝を守るためにエレメンタルが現れた！"
#define MES_TRAP_S_BIRD "鳥の群れがあなたを取り巻いた！"
#define MES_TRAP_S_H_DEMON "炎と硫黄の雲の中に悪魔が姿を現した！"
#define MES_TRAP_S_H_DRAGON "暗闇にドラゴンの影がぼんやりと現れた！"
#define MES_TRAP_S_HYBRID "奇妙な姿の怪物が襲って来た！"
#define MES_TRAP_S_VORTEX "渦巻が合体し、破裂した！"
#define MES_TRAP_E_RUNE "恐ろしい声が響いた:  「暗闇が汝をつつまん！」"
#define MES_TRAP_EXPLOSIVE "突然、箱が爆発した！箱の中の物はすべて粉々に砕け散った！"
#define MES_TRAP_SCATTER "宝箱の中身はダンジョンじゅうに散乱した！"
#define MES_TRAP_TELEPORT "テレポート・トラップにひっかかった！"
#define MES_TRAP_TY_CURSE "何かがピカッと光った！"
#define MES_TRAP_ARMAGEDDON "突然天界の戦争に巻き込まれた！"
#define MES_TRAP_ALARM "けたたましい音が鳴り響いた！"
#define MES_TRAP_OPEN "大音響と共にまわりの壁が崩れた！"
#else

#define MES_TRAP_FOUND "You found a trap."
#define MES_TRAP_FOUND_CHEST "You have discovered a trap on the chest!"

#define MES_TRAP_DOOR "You have fallen through a trap door!"
#define MES_TRAP_DOOR_AVOID "You fly over a trap door."

#define MES_TRAP_PIT "You fly over a pit trap."
#define MES_TRAP_PIT_AVOID "You have fallen into a pit!"
#define MES_TRAP_SPIKED_PIT "You have fallen into a spiked pit!"
#define MES_TRAP_SPIKED_PIT_AVOID "You fly over a spiked pit."
#define MES_TRAP_INPILED "You are impaled!"
#define MES_TRAP_POISON_INPILED "You are impaled on poisonous spikes!"

#define MES_TRAP_BLIND "A black gas surrounds you!"
#define MES_TRAP_CONFUSE "A gas of scintillating colors surrounds you!"
#define MES_TRAP_POISON "A pungent green gas surrounds you!"
#define MES_TRAP_SLEPT "A strange white mist surrounds you!"

#define MES_TRAP_NEEDLE "A small needle has pricked you!"
#define MES_TRAP_DARTS "A small dart hits you!"
#define MES_TRAP_DARTS_MISSED "A small dart barely misses you."
#define MES_TRAP_FIRE "You are enveloped in flames."
#define MES_TRAP_ACID "You are splashed with acid."
#define MES_TRAP_ACID_FLOW "Suddenly, the room is filled with acid."
#define MES_TRAP_POISON_FLOW "Suddenly, the room is filled with poisonous liquid."
#define MES_TRAP_PIRANHAS "Suddenly, the room is filled with water with piranhas!"

#define MES_TRAP_POISONOUS_GAS "A puff of green gas surrounds you!"
#define MES_TRAP_PARALYZE_GAS "A puff of yellow gas surrounds you!"
#define MES_TRAP_SUMMONING "You are enveloped in a cloud of smoke!"
#define MES_TRAP_S_ELEMENTAL "Elemental beings appear to protect their treasures!"
#define MES_TRAP_S_BIRD "A storm of birds swirls around you!"
#define MES_TRAP_S_H_DEMON "Demons materialize in clouds of fire and brimstone!"
#define MES_TRAP_S_H_DRAGON "Draconic forms loom out of the darkness!"
#define MES_TRAP_S_HYBRID "Creatures strange and twisted assault you!"
#define MES_TRAP_S_VORTEX "Vortices coalesce and wreak destruction!"
#define MES_TRAP_E_RUNE "Hideous voices bid:  'Let the darkness have thee!'"
#define MES_TRAP_EXPLOSIVE "There is a sudden explosion! Everything inside the chest is destroyed!"
#define MES_TRAP_SCATTER  "The contents of the chest scatter all over the dungeon!"
#define MES_TRAP_TELEPORT "You hit a teleport trap!"
#define MES_TRAP_TY_CURSE "There is a flash of shimmering light!"
#define MES_TRAP_ARMAGEDDON "Suddenly, you are surrounded by immotal beings!"
#define MES_TRAP_ALARM "An alarm sounds!"
#define MES_TRAP_OPEN "Suddenly, surrounding walls are opened!"
#endif

#if JP
#define MES_COMPLETE_QUEST "クエストを達成した。"
#define MES_CURRENT_QUEST "《遂行中のクエスト》"
#define MES_COMPLETED_QUEST "《達成したクエスト》"
#define MES_FAILED_QUEST "《失敗したクエスト》"
#define MES_REMAINING_QUEST "《残っているクエスト》"

#define MES_FAILED "失敗した。"
#define MES_ENCHANTMENT_FAILED "強化に失敗した。"
#define MES_IMPROVEMENT_FAILED "改良に失敗した。"
#define MES_RESIST_THE_EFFECT "しかし効力を跳ね返した。"
#define MES_IS_IMMUNE "には完全な耐性がある。"
#define MES_RESISTED "には耐性がある。"
#define MES_IS_UNAFFECTED "には効果がなかった。"
#define MES_NO_EFFECT "効果がなかった。"
#define MES_IS_CONFUSED "は目が見えない"
#define MES_IS_BLIND "は混乱している。"
#define MES_CREATURE_IN_THE_WAY "クリーチャーが立ちふさがっている。"
#define MES_MANA_RECOVERLY "頭がハッキリとした。"
#define MES_SUCCESS_PICKING "鍵をはずした。"
#define MES_FAILED_PICKING "鍵をはずせなかった。"
#define MES_FOUND_SOMETHING "何かを発見した！"

#define MES_BREAK_P_RUNE "守りのルーンが壊れた！"
#define MES_BREAK_E_RUNE "ルーンが爆発した！"
#define MES_DISARM_E_RUNE "爆発のルーンは解除された。"

#define MES_HEAR_NOISE "何かが聞こえた。"

#define MES_NO_STUDY_SPELL "読める呪文がない。"
#define MES_NO_STUDY_TECH "学べる技がない。"

#define MES_BLACK_AURA_TO_OBJECT "恐怖の暗黒オーラがあなたの%sを包み込んだ！"
#define MES_REMOVED_OBJECT_CURSE "誰かに見守られているような気がする。"
#define MES_COMBAT_TALK_STAIR "「なんだこの階段は！？」"
#define MES_PERILOUS_SECRET "石を制御できない。"
#define MES_RESISTED_FEAR "しかし恐怖に侵されなかった。"
#define MES_NO_MONEY "お金が足りません。"
#define MES_NO_MONEY_INSULT "おい！金がないじゃないか！出ていけ！"
#define MES_BOUNTY_1 "%s を換金しますか？"

#define MES_TRAP_WARNING "* 注意:この先はトラップの感知範囲外です！ *"
#define MES_TRAP_DESTROY "トラップを粉砕した。"

#define MES_LITE_AREA "光が辺りを覆った。"
#define MES_UNLITE_AREA "暗闇が辺りを覆った。"

#define MES_DESTROY "燃えるような閃光が発生した！"

#define MES_FAILED_RUNAWAY "うまく逃げられなかった。"

#define MES_DAMAGE_DENSITY "体の分子が分解した気がする！"
#define MES_ROCK_CRUSHING(CREATURE) "%sは崩れた岩に押し潰された！", (CREATURE)->name

#define MES_HAYAGAKE_PREVENT "ここでは素早く動けない。"
#define MES_ATTEMPT_ANYWAY "それでも挑戦しますか? "

#define MES_IMPROVE_LIMIT "そのアイテムはこれ以上改良できない。"

#define MES_NO_HAPPEN "何も起こらなかった。"
#define MES_RIDING_FAILED "うまく乗れなかった。"
#define MES_CONVERT_FAILED "変換に失敗した。"
#define MES_NO_DICRECTION_CREATURE "その方向にはクリーチャーはいません。"
#define MES_FALL_RIDING "乗騎から地面に落とされた。"

#define MES_FIND_TRAP "トラップを発見した。"
#define MES_FIND_DOOR "隠しドアを発見した。"


#define MES_SEE_OBJECT "%sがある。"
#define MES_GET_MONEY " $%ld の価値がある%sを見つけた。"
#define MES_PACK_WHICH_PICK "どれを拾いますか？"
#define MES_PACK_NO_ROOM "ザックには%sを入れる隙間がない。"
#define MES_PACK_NO_ROOM_FLOOR "ザックには床にあるどのアイテムも入らない。"
#define MES_PACK_OVERFLOW "ザックからアイテムがあふれた！"
#define MES_PACK_REORDER "ザックの中のアイテムを並べ直した。"
#define MES_PACK_COMBINE "ザックの中のアイテムをまとめ直した。"
#define MES_STAFF_UNSTACK "杖をまとめなおした。"

#define MES_PROJECTILE_HITS "%sが%sに命中した。"

#define MES_FIELD_MAGIC_FAILED1 "床上のアイテムが呪文を跳ね返した。"

#define MES_CALL_ARTIFACT "このアーティファクトを何と名付けますか？"

#define MES_TARGET_SET "ターゲット決定。"
#define MES_TARGET_ABORT "ターゲット解除。"
#define MES_TARGET_CANCELED "中断した。"

#define MES_GOODBYE(NAME) ("さようなら、%s!", (NAME))
#else

#define MES_COMPLETE_QUEST "You completed the quest."
#define MES_CURRENT_QUEST "< Current Quest >"
#define MES_COMPLETED_QUEST "< Completed Quest >"
#define MES_FAILED_QUEST "< Failed Quest >"
#define MES_REMAINING_QUEST "< Remaining Quest >"

#define MES_FAILED "Failed."
#define MES_ENCHANTMENT_FAILED "The enchantment failed."
#define MES_IMPROVEMENT_FAILED "The improvement failed."
#define MES_RESIST_THE_EFFECT "You resist the effects."
#define MES_IS_IMMUNE "is immune."
#define MES_RESISTED "resists."
#define MES_IS_UNAFFECTED "You are unaffected."
#define MES_NO_EFFECT "There is no effect."
#define MES_IS_CONFUSED "is confused"
#define MES_IS_BLIND "is blind!"
#define MES_CREATURE_IN_THE_WAY "There is a creature in the way."
#define MES_MANA_RECOVERLY "You feel your head clear."
#define MES_SUCCESS_PICKING "You have picked the lock."
#define MES_FAILED_PICKING "You failed to pick the lock."
#define MES_FOUND_SOMETHING "You have found something!"

#define MES_BREAK_P_RUNE "The rune of protection is broken!"
#define MES_BREAK_E_RUNE "The rune explodes!"
#define MES_DISARM_E_RUNE "An explosive rune was disarmed."
#define MES_HEAR_NOISE "You hear noise."

#define MES_NO_STUDY_SPELL "No spells to browse."
#define MES_NO_STUDY_TECH "No techniques to browse."

#define MES_BLACK_AURA_TO_OBJECT "A terrible black aura blasts your %s!"
#define MES_REMOVED_OBJECT_CURSE "You feel as if someone is watching over you."
#define MES_COMBAT_TALK_STAIR "'What's a fucking stair!?'"
#define MES_PERILOUS_SECRET "You are too weak to control the stone!"
#define MES_RESISTED_FEAR "You stand your ground!"
#define MES_NO_MONEY "You do not have the gold."
#define MES_NO_MONEY_INSULT "Hey! You don't have the gold - get out of here!"
#define MES_BOUNTY_1 "Convert %s into money?"

#define MES_TRAP_WARNING "*Leaving trap detect region!*"
#define MES_TRAP_DESTROY "You destroyed the trap."

#define MES_LITE_AREA "You are surrounded by a light."
#define MES_UNLITE_AREA "Darkness surrounds you."

#define MES_DESTROY "There is a searing blast of light!"

#define MES_FAILED_RUNAWAY "You are failed to run away."

#define MES_DAMAGE_DENSITY "Your molecules feel disrupted!"
#define MES_ROCK_CRUSHING "%s %s being crushed!", (CREATURE)->name, is_player(CREATURE) ? "are" : "is"

#define MES_HAYAGAKE_PREVENT "You cannot run in here."
#define MES_ATTEMPT_ANYWAY "Attempt it anyway? "

#define MES_IMPROVE_LIMIT "This item is no more able to be improved."

#define MES_NO_HAPPEN "Nothing happens."
#define MES_CONVERT_FAILED "You failed to convert."
#define MES_RIDING_FAILED "You failed to ride."
#define MES_NO_DICRECTION_CREATURE "There is no creature."

#define MES_FALL_RIDING "You have fallen from your riding pet."

#define MES_FIND_TRAP "You have found a trap."
#define MES_FIND_DOOR "You have found a secret door."

#define MES_SEE_OBJECT "You see %s."
#define MES_GET_MONEY "You collect %ld gold pieces worth of %s."
#define MES_PACK_WHICH_PICK "Get which item? "
#define MES_PACK_NO_ROOM "You have no room for %s."
#define MES_PACK_NO_ROOM_FLOOR "You have no room for any of the objects on the floor."
#define MES_PACK_OVERFLOW "Your pack overflows!"
#define MES_PACK_REORDER "You reorder some items in your pack."
#define MES_PACK_COMBINE "You combine some items in your pack."
#define MES_STAFF_UNSTACK "You unstack your staff."

#define MES_PROJECTILE_HITS "The %s hits %s."

#define MES_FIELD_MAGIC_FAILED1 "The object resists the spell."

#define MES_CALL_ARTIFACT "What do you want to call the artifact?"

#define MES_TARGET_SET "Target selected."
#define MES_TARGET_ABORT "Target aborted."
#define MES_TARGET_CANCELED "Canceled."

#define MES_GOODBYE(NAME) ("Goodbye, %s!", (NAME))
#endif

#ifdef JP
#define MES_ARENA_READY "試合開始！"
#define MES_ARENA_LIMIT "アリーナが魔法を吸収した！"
#define MES_ARENA_ITEM_LIMIT "アリーナではアイテムを使えない！"
#define MES_ARENA_BERSERK "狂戦士化していて頭が回らない！"
#define MES_ARENA_WON "勝利！チャンピオンへの道を進んでいる。"
#define MES_ARENA_BECAME_CHAMP "素晴らしい！君こそ真の勝利者だ。"
#define MES_ARENA_EXTRA "君のために最強の挑戦者を用意しておいた。"
#define MES_ARENA_EXTRA_ASK "挑戦するかね？"
#define MES_ARENA_EXTRA_CANCELED "残念だ。"
#define MES_ARENA_CHALLENGE(NAME) "%s に挑戦するものはいないか？", (NAME)
#define MES_ARENA_LOST(TARGET) "あなたは%sの前に敗れ去った。", (TARGET)->name
#define MES_ARENA_FEEL_GLORY "あなたはアリーナに入り、しばらくの間栄光にひたった。"
#define MES_ARENA_NO_PET "ペットに乗ったままではアリーナへ入れさせてもらえなかった。"
#define MES_ARENA_YOU_ARE_WINNER "あなたは勝利者だ。 アリーナでのセレモニーに参加しなさい。"
#define MES_ARENA_CLEARED "あなたはすべての敵に勝利した。"
#define MES_ARENA_WINNER "アリーナの優勝者！\nおめでとう！あなたは全ての敵を倒しました。\n賞金として $1,000,000 が与えられます。"
#else
#define MES_ARENA_READY "Ready..Fight!"
#define MES_ARENA_LIMIT "The arena absorbs all attempted magic!"
#define MES_ARENA_ITEM_LIMIT "You're in the arena now. This is hand-to-hand!"
#define MES_ARENA_WON "Victorious! You're on your way to becoming Champion."
#define MES_ARENA_BECAME_CHAMP "You are a Genuine Champion!"
#define MES_ARENA_BERSERK "You cannot think directly!"
#define MES_ARENA_EXTRA "The strongest challenger is waiting for you."
#define MES_ARENA_EXTRA_ASK "Do you fight? "
#define MES_ARENA_EXTRA_CANCELED "We are disappointed."
#define MES_ARENA_CHALLENGE(NAME) "Do I hear any challenges against: %s", (NAME)
#define MES_ARENA_LOST(TARGET) "You are beaten by %s.", (TARGET)->name;
#define MES_ARENA_FEEL_GLORY "You enter the arena briefly and bask in your glory."
#define MES_ARENA_NO_PET "You don't have permission to enter with pet."
#define MES_ARENA_YOU_ARE_WINNER "You are victorious. Enter the arena for the ceremony."
#define MES_ARENA_CLEARED "You have won against all foes."
#define MES_ARENA_WINNER "Arena Victor!\nCongratulations!  You have defeated all before you.\nFor that, receive the prize: 1,000,000 gold pieces"
#endif

#ifdef JP
#define MES_FIELD_UMBUSHED "襲撃だ！"
#define MES_FIELD_ASK_CHAOS "本当に〈混沌〉の領域に入りますか？"
#define MES_FIELD_START "あなたは混沌の地平を歩み始めた…"
#define MES_FIELD_TOO_NEAR_ENEMY "敵がすぐ近くにいるときは混沌の地平に入れない！"
#define MES_FIELD_LEAVE_SERVANT "ペットを置いて混沌の地平に入りますか？"
#else
#define MES_FIELD_UMBUSHED "You are ambushed !"
#define MES_FIELD_ASK_CHAOS "Really want to enter territory of chaos? "
#define MES_FIELD_START "You begin to walk the horizon of chaos..."
#define MES_FIELD_TOO_NEAR_ENEMY "You cannot enter global map, since there is some creatures nearby!"
#define MES_FIELD_LEAVE_SERVANT "Do you leave your pets behind? "
#endif

#ifdef JP
#define MES_SMITH_WHICH_STUFF "どのアイテムから作りますか？ "
#define MES_SMITH_NO_STUFF "材料を持っていない。"
#define MES_SMITH_WHICH_AMMO "[S]弾, [A]矢, [B]クロスボウの矢 :"
#define MES_SMITH_WHICH_DRAIN "どのアイテムから抽出しますか？"
#define MES_SMITH_NO_DRAIN "抽出できるアイテムがありません。"
#define MES_SMITH_DRAIN_ASK(OBJECT) "本当に%sから抽出してよろしいですか？", (OBJECT)
#define MES_SMITH_DRAIN_NOTHING "エッセンスは抽出できませんでした。"
#define MES_SMITH_DRAINED_ESSENCE "抽出したエッセンス:"
#define MES_SMITH_WHICH_ENCHANT "何を付加しますか:"
#define MES_SMITH_WHICH_ABILITY "('*'で一覧, ESCで中断) どの能力を付加しますか？"
#define MES_SMITH_WHICH_ERASE "どのアイテムのエッセンスを消去しますか？"
#define MES_SMITH_ERASED "エッセンスを取り去った。"
#define MES_SMITH_NO_ERASE "エッセンスを付加したアイテムがありません。"
#define MES_SMITH_MADE(OBJECT) "%sを作った。", (OBJECT)->name
#define MES_SMITH_USE_ESSENCE "エッセンスを%d個使用します。"
#define MES_SMITH_ASK_ADD(ESSENCE) "%sを付加しますか？ ", (ESSENCE)
#define MES_SMITH_NO_ESSENCE "エッセンスが足りない。"
#define MES_SMITH_NEED_ESSENCE(NUM) "エッセンスが%d必要です。", (NUM)
#define MES_SMITH_CANNOT_INCREASE "このアイテムの能力修正を強化することはできない。"
#define MES_SMITH_PVAL_BECOME_ONE "修正値は1になります。よろしいですか？"
#define MES_SMITH_HOW_MANY_ENCHANT(NUM) "いくつ付加しますか？ (1-%d)", (NUM)
#define MES_SMITH_ADDED_ESSENCE(OBJECT, TYPE) "%sに%sの能力を付加しました。", (OBJECT)->name, (TYPE)
#else
#define MES_SMITH_WHICH_STUFF "Convert which item? "
#define MES_SMITH_NO_STUFF "You have no item to convert."
#define MES_SMITH_WHICH_AMMO "Create [S]hots, Create [A]rrow or Create [B]olt ?"
#define MES_SMITH_WHICH_DRAIN "Extract from which item? "
#define MES_SMITH_NO_DRAIN "You have nothing you can extract from."
#define MES_SMITH_DRAIN_ASK(OBJECT) "Really extract from %s? ", (OBJECT)
#define MES_SMITH_DRAIN_NOTHING "You were not able to extract any essence."
#define MES_SMITH_DRAINED_ESSENCE "Extracted essences:"
#define MES_SMITH_WHICH_ENCHANT "Command :"
#define MES_SMITH_WHICH_ABILITY "(*=List, ESC=exit) Add which ability? "
#define MES_SMITH_WHICH_ERASE "Remove from which item? "
#define MES_SMITH_ERASED "You removed all essence you have added."
#define MES_SMITH_NO_ERASE "You have nothing to remove essence."
#define MES_SMITH_MADE(OBJECT) "You made %s", (OBJECT)->name
#define MES_SMITH_USE_ESSENCE "It will take %d essences."
#define MES_SMITH_ASK_ADD(ESSENCE) "Add the abilitiy of %s? ", (ESSENCE)
#define MES_SMITH_NO_ESSENCE "You don't have enough essences."
#define MES_SMITH_NEED_ESSENCE(NUM) "It will take %d essences.", (NUM)
#define MES_SMITH_CANNOT_INCREASE "You cannot increase magic number of this item."
#define MES_SMITH_PVAL_BECOME_ONE "The magic number of this weapon will become 1. Are you sure? "
#define MES_SMITH_HOW_MANY_ENCHANT(NUM) "Enchant how many? (1-%d)", (NUM)
#define MES_SMITH_ADDED_ESSENCE(OBJECT, TYPE) "You have added ability of %s to %s.", (OBJECT)->name, (TYPE)
#endif


#ifdef JP
#define MES_PROBING_START "調査中..."
#define MES_PROBING_FORMAT "%s 属性:%s 性別:%s HP:%d/%d AC:%d 速度:%s%d 腕:%d 知:%d 賢:%d 器:%d 耐:%d 魅:%d 経験:"
#else
#define MES_PROBING_START "Probing..."
#define MES_PROBING_FORMAT "%s align:%s sex:%s HP:%d/%d AC:%d speed:%s%d STR:%d INT:%d WIS:%d DEX:%d CON:%d CHA:%d exp:"
#endif

#ifdef JP
#define MES_FATAL_SPOT "%sの急所に突き刺さった！"
#define MES_SUICIDE_BOMBER "爆発して粉々になった。"
#define MES_CARSE_OF_BLOOD_DEAD "%^sは死の間際に恐ろしい血の呪いを%^sにかけた！"
#define MES_LOST_LIFE1 "生命力を少し吸い取られた気がする！"
#define MES_LOST_LIFE2 "生命力が体から吸い取られた気がする！"
#define MES_GUARD_LIFE "しかし自己の生命力を守りきった！"
#define MES_BLOE_AWAY "%sを吹き飛ばした！"
#define MES_CAMP_GET_ANGRY "%sは怒った！"
#define MES_HOSTILE "%^sは敵にまわった！"
#define MES_BOUNTY_DEAD "%sの首には報奨がかかっている。"
#else
#define MES_FATAL_SPOT "Your shot sticked on a fatal spot of %s!"
#define MES_SUICIDE_BOMBER "%^s explodes into tiny shreds."
#define MES_CARSE_OF_BLOOD_DEAD "On death and dying, %^s puts a terrible blood curse on %^s!"
#define MES_LOST_LIFE1 "You feel your life slipping away!"
#define MES_LOST_LIFE2 "You feel your life draining away..."
#define MES_GUARD_LIFE "You keep hold of your life force!"
#define MES_BLOE_AWAY "You blow %s away!"
#define MES_CAMP_GET_ANGRY "%^s gets angry!"
#define MES_HOSTILE "%^s suddenly becomes hostile!"
#define MES_BOUNTY_DEAD "There is a price on %s's head."
#endif

#ifdef JP
#define MES_RESTORE_SOME_MP "少し頭がハッキリした。"
#else
#define MES_RESTORE_SOME_MP "You feel your head clear a little."
#endif

#ifdef JP
#define MES_EARTHQUAKE "地面が揺れた。"
#else
#define MES_EARTHQUAKE "The ground trembles."
#endif


// messages of detection

#ifdef JP

#define MES_DETECT_TRAPS "トラップの存在を感じとった！"
#define MES_DETECT_DOORS "ドアの存在を感じとった！"
#define MES_DETECT_STAIRS "階段の存在を感じとった！"
#define MES_DETECT_B_TREASURE "埋蔵された財宝の存在を感じとった！"
#define MES_DETECT_TREASURE "財宝の存在を感じとった！"
#define MES_DETECT_OBJECTS "アイテムの存在を感じとった！"
#define MES_DETECT_M_OBJECTS "魔法のアイテムの存在を感じとった！"
#define MES_DETECT_CREATURES "クリーチャーの存在を感じとった！"
#define MES_DETECT_I_CREATURES "透明な生物の存在を感じとった！"
#define MES_DETECT_E_CREATURES "邪悪なる生物の存在を感じとった！"
#define MES_DETECT_S_CREATURES "殺気を感じとった！"
#define MES_DETECT_SOMEONE "%sの存在を感じとった！"
#define MES_DETECT_NOLIVING "自然でないクリーチャーの存在を感じた！"
#define MES_DETECT_ENLIGHTMENT "自分の置かれている状況が脳裏に浮かんできた..."
#define MES_DETECT_MORE_ENLIGHTMENT "更なる啓蒙を感じた..."
#else

#define MES_DETECT_TRAPS "You sense the presence of traps!"
#define MES_DETECT_DOORS "You sense the presence of doors!"
#define MES_DETECT_STAIRS "You sense the presence of stairs!"
#define MES_DETECT_B_TREASURE "You sense the presence of buried treasure!"
#define MES_DETECT_TREASURE "You sense the presence of treasure!"
#define MES_DETECT_OBJECTS "You sense the presence of objects!"
#define MES_DETECT_M_OBJECTS "You sense the presence of magic objects!"
#define MES_DETECT_CREATURES "You sense the presence of creatures!"
#define MES_DETECT_I_CREATURES "You sense the presence of invisible creatures!"
#define MES_DETECT_E_CREATURES "You sense the presence of evil creatures!"
#define MES_DETECT_S_CREATURES "You sense the presence of someone's mind!"
#define MES_DETECT_SOMEONE "You sense the presence of %s!"
#define MES_DETECT_NOLIVING "You sense the presence of unnatural beings!"
#define MES_DETECT_ENLIGHTMENT "An image of your surroundings forms in your mind..."
#define MES_DETECT_MORE_ENLIGHTMENT "You begin to feel more enlightened..."
#endif


#ifdef JP
#define MES_FEELING_QUESTDUN "典型的なクエストのダンジョンのようだ。"
#define MES_FEELING_WILD_POINT "何かありそうな荒野のようだ。"
#define MES_FEELING_TOWN "典型的な町のようだ。"
#define MES_FEELING_WILD "典型的な荒野のようだ。"
#else
#define MES_FEELING_QUESTDUN "Looks like a typical quest level.";
#define MES_FEELING_WILD_POINT "Looks like a strange wilderness."
#define MES_FEELING_TOWN "Looks like a typical town."
#define MES_FEELING_WILD "Looks like a typical wilderness."
#endif

#ifdef JP
#define MES_CANNOT_GO_MORE "それ以上先には進めない。"
#else
#define MES_CANNOT_GO_MORE "You cannot go any more."
#endif

#ifdef JP
#define MES_BLDG_CLOSED "(閉店)"
#define MES_BLDG_NO_BLDG "ここには建物はない。"
#define MES_BLDG_LOSER "「敗者に用はない。」"
#define MES_BLDG_ARENA_FIGHT "ゲートは閉まっている。相手があなたを待っている！"
#define MES_BLDG_NO_RIGHT "それを選択する権利はありません！"
#define MES_BULD_EXIT "建物を出る"

#define MES_BLDG_GET_PRICE "賞金 %ld＄を手に入れた。"
#define MES_BLDG_TODAY_TARGET1(TARGET) "ターゲット： %s", (TARGET)
#define MES_BLDG_TODAY_TARGET2(PRICE) "死体 ---- $%d", (PRICE)
#define MES_BLDG_TODAY_TARGET3(PRICE) "骨   ---- $%d", (PRICE)
#define MES_BLDG_TSUCHINOKO1 "一獲千金の大チャンス！！！\nターゲット：幻の珍獣「ツチノコ」"
#define MES_BLDG_TSUCHINOKO2 "生け捕り ---- $1,000,000\n死体     ----   $200,000\n骨       ----   $100,000"
#define MES_BOUNTY_HAND_OVER(CORPSE) "%sを渡しますか？", (CORPSE)
#define MES_BOUNTY_EARN_POINT(NUM) "これで合計 %d ポイント獲得しました。", (NUM)
#define MES_BOUNTY_GET_REWARD(OBJECT, SYM) "%s(%c)を貰った。", (OBJECT), (SYM)
#define MES_BOUNTY_NOTHING "賞金を得られそうなものは持っていなかった。"

#else
#define MES_BLDG_CLOSED "(closed)"
#define MES_BLDG_NO_BLDG "You see no building here."
#define MES_BLDG_LOSER "'There's no place here for a LOSER like you!'"
#define MES_BLDG_ARENA_FIGHT "The gates are closed.  The enemy awaits!"
#define MES_BLDG_NO_RIGHT "You have no right to choose that!"
#define MES_BULD_EXIT "Exit building"

#define MES_BLDG_GET_PRICE "You get %ldgp."
#define MES_BLDG_TODAY_TARGET1(TARGET) "target: %s", (TARGET)
#define MES_BLDG_TODAY_TARGET2(PRICE) "corpse   ---- $%d", (PRICE)
#define MES_BLDG_TODAY_TARGET3(PRICE) "skeleton ---- $%d", (PRICE)
#define MES_BLDG_TSUCHINOKO1 "Big chance to quick money!!!\ntarget: the rarest animal 'Tsuchinoko'"
#define MES_BLDG_TSUCHINOKO2 "catch alive ---- $1,000,000\ncorpse      ----   $200,000\nbones       ----   $100,000"
#define MES_BOUNTY_HAND_OVER(CORPSE) "Hand %s over? ", (CORPSE)
#define MES_BOUNTY_EARN_POINT(NUM) "You earned %d point%s total.", (NUM), ((NUM) > 1 ? "s" : "") 
#define MES_BOUNTY_GET_REWARD(OBJECT, SYM) "You get %s (%c). ", (OBJECT), (SYM)
#define MES_BOUNTY_NOTHING "You have nothing."

#endif

#ifdef JP
#define MES_FEATURE_NO_UP_STAIR "ここには上り階段が見当たらない。"
#define MES_FEATURE_NO_DOWN_STAIR "ここには下り階段が見当たらない。"
#define MES_FEATURE_UP_STAIR "上の階に降りた。"
#define MES_FEATURE_DOWN_STAIR "下の階に降りた。"
#define MES_FEATURE_CREATE_STAIR "魔法の階段が現れた..."
#define MES_FEATURE_TRAP_DOOR_SELF "わざと落とし戸に落ちた。"
#define MES_FEATURE_ENTER_DUNGEON(MES) "%sへ入った。", (MES)

#define MES_FEATURE_STUCK "%sはがっちりと閉じられているようだ。"
#define MES_FEATURE_CLOSED_DUNGEON "ダンジョンの入口は塞がれている！"
#define MES_FEATURE_CHECK_DUNGEON "ここには%sの入り口(%d階相当)があります"
#define MES_FEATURE_ASK_ENTER_DUNGEON "本当にこのダンジョンに入りますか？"

#define MES_PATTERN_LIMIT1 "パターンの上を歩き始めると、全てを歩かなければなりません。いいですか？"
#define MES_PATTERN_LIMIT2 "パターンの上を歩くにはスタート地点から歩き始めなくてはなりません。"
#define MES_PATTERN_LIMIT3 "パターンの上は正しい順序で歩かねばなりません。"
#define MES_PATTERN_LIMIT4 "パターンを踏み外してはいけません。"
#define MES_PATTERN_GOAL "「パターン」のこの部分は他の部分より強力でないようだ。"
#define MES_PATTERN_WRECKED "パターンを血で汚してしまった！何か恐ろしい事が起こりそうだ！"

#else
#define MES_FEATURE_NO_UP_STAIR "I see no up staircase here."
#define MES_FEATURE_NO_DOWN_STAIR "I see no down staircase here."
#define MES_FEATURE_UP_STAIR "You enter the up staircase."
#define MES_FEATURE_DOWN_STAIR "You enter the down staircase."
#define MES_FEATURE_CREATE_STAIR "A magical staircase appears..."
#define MES_FEATURE_TRAP_DOOR_SELF "You deliberately jump through the trap door."
#define MES_FEATURE_ENTER_DUNGEON(MES) "You entered %s.", (MES)

#define MES_FEATURE_STUCK "The %s appears to be stuck."
#define MES_FEATURE_CLOSED_DUNGEON "The entrance of this dungeon is closed!"
#define MES_FEATURE_CHECK_DUNGEON "There is the entrance of %s (Danger level: %d)"
#define MES_FEATURE_ASK_ENTER_DUNGEON "Do you really get in this dungeon? "

#define MES_PATTERN_LIMIT1 "If you start walking the Pattern, you must walk the whole way. Ok? "
#define MES_PATTERN_LIMIT2 "You must start walking the Pattern from the startpoint."
#define MES_PATTERN_LIMIT3 "You must walk the Pattern in correct order."
#define MES_PATTERN_LIMIT4 "You may not step off from the Pattern."
#define MES_PATTERN_GOAL "This section of the Pattern looks less powerful."
#define MES_PATTERN_WRECKED "You bleed on the Pattern! Something terrible happens!"
#endif

#ifdef JP
#define MES_SUMMON_NOTHING "何も現れなかった。"
#define MES_SUMMON_SERVANT "御用でございますか？我が主。"
#define MES_SUMMON_FEELING "何かが間近に現れた音がする。"
#define MES_SUMMON_FUMBLE(HOSTILE) "召喚された%sは召喚者に敵意を向けている。", (HOSTILE)
#define MES_SUMMON_INROU "「者ども、ひかえおろう！！！このお方をどなたとこころえる。」"
#else
#define MES_SUMMON_NOTHING "Nobody ever turns up..."
#define MES_SUMMON_SERVANT "'What is thy bidding... Master?'"
#define MES_SUMMON_FEELING "You hear something appear nearby."
#define MES_SUMMON_FUMBLE(HOSTILE) "The summoned %s are angry.", (HOSTILE)
#define MES_SUMMON_INROU "'WHO do you think this person is! Bow your head, down your knees!'"
#endif

#ifdef JP
#define MES_OBJECT_DISENCHANTED(OBJECT) "%sは劣化してしまった。", (OBJECT)->name
#define MES_OBJECT_RESISTED_CURSE(OBJECT) "%sは恐怖の暗黒オーラを跳ね返した。", (OBJECT)->name
#define MES_OBJECT_KILLED "壊れてしまった。"
#define MES_OBJECT_DUSTED "灰になった。"
#else
#define MES_OBJECT_DISENCHANTED(OBJECT) "Your %s was disenchanted.", (OBJECT)
#define MES_OBJECT_RESISTED_CURSE(OBJECT) "Your %s resists the terrible black aura.", (OBJECT)->name
#define MES_OBJECT_KILLED (plural ? " are destroyed!" : " is destroyed!")
#define MES_OBJECT_DUSTED (plural ? " become dust." : " becomes dust.")
#endif

#ifdef JP
#define MES_DIGGING_PERMANENT1 "この岩は硬すぎて掘れないようだ。"
#define MES_DIGGING_PERMANENT2 "そこは掘れない!"
#define MES_DIGGING_DONE1(NAME) ("%sをくずした。", (NAME))
#define MES_DIGGING_DONE2(NAME) ("%sをくずしている。", (NAME))
#define MES_DIGGING_CUT(TARGET) "%sを切っている。", (TARGET)
#define MES_DIGGING_CUT_DONE(TARGET) "%sを切り払った。", (TARGET)
#define MES_DIGGING_TUNNEL(TARGET) "%sに穴を掘っている。", (TARGET)
#define MES_DIGGING_TUNNEL_DONE "穴を掘り終えた。"
#define MES_DIGGING_TUNNEL_DOOR "ドアは掘れない。"
#define MES_DIGGING_TUNNEL_DISABLE "そこは掘れない。"
#else
#define MES_DIGGING_PERMANENT1 "This seems to be permanent rock."
#define MES_DIGGING_PERMANENT2 "You can't tunnel through that!"
#define MES_DIGGING_DONE1(NAME) ("You have removed the %s.", (NAME))
#define MES_DIGGING_DONE2(NAME) ("You dig into the %s.", (NAME))
#define MES_DIGGING_CUT(TARGET) "You chop away at the %s.", (TARGET)
#define MES_DIGGING_CUT_DONE(TARGET) "You have cleared away the %s.", (TARGET)
#define MES_DIGGING_TUNNEL(TARGET) "You tunnel into the %s.", (TARGET)
#define MES_DIGGING_TUNNEL_DONE "You have finished the tunnel."
#define MES_DIGGING_TUNNEL_DOOR "You cannot tunnel through doors."
#define MES_DIGGING_TUNNEL_DISABLE "You can't tunnel through that."
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
#define MES_TELEPORT_LEVEL_ASK "この階を去りますか？"
#define MES_TELEPORT_NO_TOWN "まだ行けるところがない。"
#define MES_TELEPORT_WHICH_TOWN "どこに行きますか:"
#define MES_TELEPORT_LEVEL_UP(TARGET) "%^sは天井を突き破って宙へ浮いていく。", (TARGET)
#define MES_TELEPORT_LEVEL_DOWN(TARGET) "%^sは床を突き破って沈んでいく。", (TARGET)
#define MES_RECALL_ASK "帰還の力を使いますか？"
#define MES_RECALL_STARTING "回りの大気が張りつめてきた..."
#define MES_RECALL_CANCEL "張りつめた大気が流れ去った..."
#define MES_RECALL_TRUMP_TOWER "トランプタワーで"
#else
#define MES_TELEPORT_LEVEL_ASK "Leave this level? "
#define MES_TELEPORT_NO_TOWN "You have not yet visited any town."
#define MES_TELEPORT_WHICH_TOWN "Which town you go: "
#define MES_TELEPORT_LEVEL_UP(TARGET) "%^s rise%s up through the ceiling.", (TARGET), (m_idx <= 0) ? "" : "s"
#define MES_TELEPORT_LEVEL_DOWN(TARGET) "%^s sink%s through the floor.", m_name, (m_idx <= 0) ? "" : "s"
#define MES_RECALL_ASK "Activate recall? "
#define MES_RECALL_STARTING "The air about you becomes charged..."
#define MES_RECALL_CANCEL "A tension leaves the air around you..."
#define MES_RECALL_TRUMP_TOWER "at Trump Tower"
#endif

#if JP
#define MES_ALCHEMY_WHICH_OBJECT "どのアイテムを金に変えますか？"
#define MES_ALCHEMY_NO_OBJECT "金に変えられる物がありません。"
#define MES_ALCHEMY_ASK(OBJECT) "本当に%sを金に変えますか？", (OBJECT)
#define MES_ALCHEMY_FAILED(OBJECT) "%sを金に変えることに失敗した。", (OBJECT)
#define MES_ALCHEMY_NO_PRICE(OBJECT) "%sをニセの金に変えた。", (OBJECT)
#define MES_ALCHEMY_SUCCESS(OBJECT, PRICE) "%sを＄%d の金に変えた。", (OBJECT), (PRICE)
#else
#define MES_ALCHEMY_WHICH_OBJECT "Turn which item to gold? "
#define MES_ALCHEMY_NO_OBJECT "You have nothing to turn to gold."
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
#define COD_POTION_OF_DETONATION "爆発の薬"
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
#define COD_EQUIPED_DAMAGE(OBJECT_NAME) "%sを装備したダメージ", OBJECT_NAME
#define COD_HELLFIRE_RISK "地獄の劫火の呪文を唱えた疲労"
#define COD_JOJ_RISK "審判の宝石"
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
#define COD_POTION_OF_DETONATION "a potion of Detonation"
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
#define COD_EQUIPED_DAMAGE(OBJECT_NAME) "wielding %s", OBJECT_NAME
#define COD_HELLFIRE_RISK "the strain of casting Hellfire"
#define COD_JOJ_RISK "the Jewel of Judgement"
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

#define TEXT_FILES_HELP "jhelp.hlp"
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

#define TEXT_FILES_HELP "help.hlp"
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
	#define PROMPT_FIND "検索(^I:持ち物 ^L:破壊された物): "
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
	#define PROMPT_FIND "Search key(^I:inven ^L:destroyed): "
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
	#define SKILL_NAME_PSYONIC "超能力"
	#define SKILL_NAME_IMITATION "ものまね"
	#define SKILL_NAME_HISSATSU "必殺剣"
	#define SKILL_NAME_MIRROR_M "鏡魔法"
	#define SKILL_NAME_NINJUTSU "忍術"
	#define SKILL_NAME_PRAYER "祈り"
	#define SKILL_NAME_FORCE "錬気術"
	#define SKILL_NAME_BRUTAL "狂奔"
	#define SKILL_NAME_SNIPING "射撃術"
#else
	#define SKILL_NAME_MAGIC "magic"
	#define SKILL_NAME_PSYONIC "psionic powers"
	#define SKILL_NAME_IMITATION "imitation"
	#define SKILL_NAME_HISSATSU "hissatsu"
	#define SKILL_NAME_MIRROR_M "mirror magic"
	#define SKILL_NAME_NINJUTSU "ninjutsu"
	#define SKILL_NAME_PRAYER "prayer"
	#define SKILL_NAME_FORCE "force"
	#define SKILL_NAME_BRUTAL "brutal power"
	#define SKILL_NAME_SNIPING "Sniping"
#endif

#ifdef JP

#define DIATY_DATE(DAY) "%d日目\n", (DAY)
#define DIATY_COUNTSTOP_DATE "*****日目\n"
#define DIARY_PLACE_ARENA "アリーナ:"
#define DIARY_PLACE_SURFACE "地上:"
#define DIARY_PLACE_QUEST "クエスト:"
#define DIARY_TITLE(CREATURE) "「%s%sの伝説」", chara_info[(CREATURE)->chara_idx].title, (CREATURE)->name
#define DIARY_NOTE "内容"

#define DIARY_CREATE_PLAYER "-------- プレイヤー作成 --------"
#define DIARY_GAMEOVER "-------- ゲームオーバー --------"
#define DIARY_SAVEQUIT "----ゲーム中断----"
#define DIARY_GAMERESTART "----ゲーム再開----"
#define DIARY_GAMEABORT "----ゲーム異常終了----"
#define DIARY_SELECTMODE "キャンペーン『%s』を選択した"
#define DIARY_SELECTSEX(SEX) "性別に%sを選択した。", (SEX)
#define DIARY_SELECT_PURERACE(RACE) "種族に%sを選択した。", (RACE)
#define DIARY_SELECT_HALFRACE(RACE1, RACE2) "種族に%sと%sの混血を選択した。", (RACE1), (RACE2)
#define DIARY_SELECT_CLASS(CLASS) "職業に%sを選択した。", (CLASS)
#define DIARY_SELECT_REALM(REALM) "魔法の領域に%sを選択した。", (REALM)
#define DIARY_SELECT_CHALA(CHALA) "性格に%sを選択した。", (CHALA)
#define DIARY_RETIRED "ダンジョンの探索から引退した。"
#define DIARY_SUICIDE "絶望して自殺した。"
#define DIARY_WINNING_SEPPUKU "勝利の後切腹した。"
#define DIARY_WIZARD "ウィザードモードに突入してスコアを残せなくなった。"
#define DIARY_UP_STAIR "階段を上った"
#define DIARY_DOWN_STAIR "階段を下りた"
#define DIARY_TRAP_DOOR "落とし戸に落ちた"
#define DIARY_PINCH "%sによってピンチに陥った。"
#define DIARY_PATRON(REWARD) "パトロンの報酬で%s", (REWARD)
#define DIARY_INN_OVER_DAY "宿屋で昼間を過ごした。"
#define DIARY_INN_MORNING "あなたはリフレッシュして目覚め、新たな日を迎えた。"
#define DIARY_INN_MORNING2 "すがすがしい朝を迎えた。"
#define DIARY_INN_EVENING "あなたはリフレッシュして目覚め、夕方を迎えた。"
#define DIARY_INN_EVENING2 "夕方を迎えた。"
#define DIARY_INN_STAY_NIGHT "宿屋で一夜を明かした。"
#define DIARY_INN_NIGHTMARE "悪夢にうなされてよく眠れなかった。"
#else

#define DIATY_DATE(DAY) "Day %d\n", (DAY)
#define DIATY_COUNTSTOP_DATE "Day *****\n"
#define DIARY_PLACE_ARENA "Arena:"
#define DIARY_PLACE_SURFACE "Surface:"
#define DIARY_PLACE_QUEST "Quest:"
#define DIARY_TITLE(CREATURE) "Legend of %s %s", chara_info[(CREATURE)->chara_idx].title, (CREATURE)->name
#define DIARY_NOTE "Diary Note"

#define DIARY_CREATE_PLAYER "-------- Create Player --------"
#define DIARY_GAMEOVER "--------   Game  Over   --------"
#define DIARY_SAVEQUIT "---- Save and Exit Game ----"
#define DIARY_GAMERESTART "---- Restart Game ----"
#define DIARY_GAMEABORT "---- Panic Save and Abort Game ----"
#define DIARY_SELECTMODE "Select Campaign '%s'"
#define DIARY_SELECTSEX(SEX) "choose %s personality.", (SEX)
#define DIARY_SELECT_PURERACE(RACE) "choose %s race.", (RACE)
#define DIARY_SELECT_HALFRACE(RACE1, RACE2) "choose %s and %s races.", (RACE1), (RACE2)
#define DIARY_SELECT_CLASS(CLASS) "choose %s class.", (CLASS)
#define DIARY_SELECT_REALM(REALM) "choose %s realm.", (REALM)
#define DIARY_SELECT_CHALA(CHALA) "choose %s.", (CHALA)
#define DIARY_RETIRED "retired exploring dungeons."
#define DIARY_SUICIDE "give up all hope to commit suicide."
#define DIARY_WINNING_SEPPUKU "did Seppuku after the winning."
#define DIARY_WIZARD "give up sending score to use wizard mode."
#define DIARY_UP_STAIR 1"climbed up the stairs to"
#define DIARY_DOWN_STAIR "climbed down the stairs to"
#define DIARY_TRAP_DOOR "You have fallen through a trap door."
#define DIARY_PINCH "A critical situation because of %s".
#define DIARY_PATRON(REWARD) "The patron rewards you with %s.", (REWARD)
#define DIARY_INN_MORNING "You awake refreshed for the new day."
#define DIARY_INN_MORNING2 "awake refreshed."
#define DIARY_INN_EVENING "You awake refreshed for the evening."
#define DIARY_INN_EVENING2 "awake refreshed."
#define DIARY_INN_OVER_DAY "stay over daytime at the inn."
#define DIARY_INN_STAY_NIGHT "stay over night at the inn."
#define DIARY_INN_NIGHTMARE "be troubled by a nightmare."
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
#define MES_DEBUG_ROOM_NUM "部屋数: %d"
#define MES_DEBUG_GLASS_ROOM "[ガラスの部屋]"
#define MES_DEBUG_ROOM_OF "[%sの部屋]"
#define MES_DEBUG_CRYPT_ROOM "[聖堂]"
#define MES_DEBUG_TRAP_PIT_ROOM "[%s%sの罠ピット]"
#define MES_DEBUG_BIAS_ARTIFACT "運の偏ったアーティファクト: %d。"
#define MES_DEBUG_NO_BIAS_ARTIFACT "偏っていないアーティファクト"
#define MES_DEBUG_FIXED_ARTIFACT(OBJECT) "固定アーティファクト(%s)", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT(OBJECT) "ランダムアーティファクト(%s)", (OBJECT)
#define MES_DEBUG_EGO_ITEM(OBJECT) "エゴアイテム(%s)", (OBJECT)
#define MES_DEBUG_NORMAL_ITEM(OBJECT) "アイテム(%s)", (OBJECT)
#define MES_DEBUG_DIE_CHOICE "死にますか?"
#define MES_DEBUG_LAKE "湖を生成。"
#define MES_DEBUG_CAVE "洞窟を生成。"
#define MES_DEBUG_ARENA "アリーナレベル"
#define MES_DEBUG_DEC_CREATURE(FROM, TO) "クリーチャー数基本値を %d から %d に減らします", (FROM), (TO)
#define MES_DEBUG_SMALL_LEVEL "小さなフロア"
#define MES_DEBUG_FLOOR_SIZE(WIDTH, HEIGHT) "[W:%d, H:%d]"
#define MES_DEBUG_VAULT(NAME) "地下室(%s)", (NAME)
#define MES_DEBUG_GREATER_VAULT(NAME) "巨大な地下室(%s)", (NAME)
#define MES_DEBUG_FAILED_VAULT "地下室を配置できません。"
#define MES_DEBUG_ARTIFACT_CANCEL(OBJECT) "伝説のアイテム (%s) は生成中に*破壊*された。", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT_CANCEL "ランダム・アーティファクトの1つは生成中に*破壊*された。"
#define MES_DEBUG_ARTIFACT_CANCEL2(OBJECT) "伝説のアイテム (%s) はストリーマーにより削除された。", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT_CANCEL2 "ランダム・アーティファクトの1つはストリーマーにより削除された。"
#define MES_DEBUG_FAILED_STREAMER "ストリーマーの生成に失敗した。"
#define MES_DEBUG_DESTROYED_FLOOR "破壊された階"
#define MES_DEBUG_FUNNY_PATTERN(FEATURE) "おかしなパターン歩行、%d。", (FEATURE)
#define MES_DEBUG_FAILED_FLOOR "ダンジョン生成に失敗"
#define MES_DEBUG_TOO_ITEM "アイテムが多過ぎる"
#define MES_DEBUG_TOO_CREATURE "クリーチャーが多過ぎる"
#define MES_DEBUG_FLOOR_RETAKE(WHY) "生成やり直し(%s)"
#else
#define MES_DEBUG_ROOM_NUM "Number of Rooms: %d"
#define MES_DEBUG_GLASS_ROOM "[Glass Room]"
#define MES_DEBUG_ROOM_OF "[Room of %s]"
#define MES_DEBUG_CRYPT_ROOM "[Crypt]"
#define MES_DEBUG_TRAP_PIT_ROOM "[Trapped creature pit (%s%s)]"
#define MES_DEBUG_BIAS_ARTIFACT "Biased artifact: %d."
#define MES_DEBUG_NO_BIAS_ARTIFACT "No biased artifact."
#define MES_DEBUG_FIXED_ARTIFACT(OBJECT) "Fixed Artifact (%s)", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT(OBJECT) "Random Artifact (%s)", (OBJECT)
#define MES_DEBUG_EGO_ITEM(OBJECT) "Ego Item (%s)", (OBJECT)
#define MES_DEBUG_NORMAL_ITEM(OBJECT) "Item (%s)", (OBJECT)
#define MES_DEBUG_DIE_CHOICE "Die?"
#define MES_DEBUG_LAKE "Lake on the level."
#define MES_DEBUG_CAVE "Cavern on level."
#define MES_DEBUG_ARENA "Arena level."
#define MES_DEBUG_DEC_CREATURE(FROM, TO) "Reduced creatures base from %d to %d", (FROM), (TO)
#define MES_DEBUG_SMALL_LEVEL "A small dungeon level."
#define MES_DEBUG_FLOOR_SIZE(WIDTH, HEIGHT) "[W:%d, H:%d]"
#define MES_DEBUG_VAULT(NAME) "Vault(%s)", (NAME)
#define MES_DEBUG_GREATER_VAULT(NAME) "Greater vault (%s)", (NAME)
#define MES_DEBUG_FAILED_VAULT "Could not place vault."
#define MES_DEBUG_ARTIFACT_CANCEL(OBJECT) "Artifact (%s) was *destroyed* during generation.", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT_CANCEL "One of the random artifacts was *destroyed* during generation."
#define MES_DEBUG_ARTIFACT_CANCEL2(OBJECT) "Artifact (%s) was deleted by streamer.", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT_CANCEL2 "One of the random artifacts was deleted by streamer."
#define MES_DEBUG_FAILED_STREAMER "Could not place streamer!"
#define MES_DEBUG_DESTROYED_FLOOR "Destroyed Floor"
#define MES_DEBUG_FUNNY_PATTERN(FEATURE) "Funny Pattern walking, %d.", (FEATURE)
#define MES_DEBUG_FAILED_FLOOR "could not place player"
#define MES_DEBUG_TOO_ITEM "too many objects"
#define MES_DEBUG_TOO_CREATURE "too many creatures"
#define MES_DEBUG_FLOOR_RETAKE(WHY) "Generation restarted (%s)"
#endif

#ifdef JP
#define MES_EFFECT_KILL_ACID "融けてしまった！"
#define MES_EFFECT_KILL_FIRE "燃えてしまった！"
#define MES_EFFECT_KILL_COLD "砕け散ってしまった！"
#define MES_EFFECT_KILL_DISINTEGRATE "蒸発してしまった！"
#define MES_EFFECT_REVIVED "生き返った。"
#define MES_EFFECT_KILL_OBJECT(OBJECT_NAME, KILL) "%sは%s", (OBJECT_NAME), (KILL)
#define MES_EFFECT_UNEFFECTED_OBJECT "%sは影響を受けなかった。", object_name
#define MES_EFFECT_CHAOS_DONE "あなたの身体はカオスの力で捻じ曲げられた！"
#define MES_EFFECT_TIME_DONE(TARGET) "%sの時の流れが逆戻りした。", (TARGET)
#define MES_EFFECT_GRAVITY_DONE "周辺の重力がゆがんだ。"
#define MES_EFFECT_MIRROR_CRUSH "鏡が割れた！"
#define MES_EFFECT_MIND_BACKFIRE "%^sの堕落した精神は攻撃を跳ね返した！"
#define MES_EFFECT_DISARM_TRAP "カチッと音がした。"
#define MES_EFFECT_CUT_ARROW "矢を斬り捨てた！"
#define MES_EFFECT_BLASTED "枯れた"
#define MES_EFFECT_SHRANK "縮んだ"
#define MES_EFFECT_MELTED "溶けた"
#define MES_EFFECT_FROZEN "凍り、砕け散った"
#define MES_EFFECT_BURN_UP "燃えた"
#define MES_EFFECT_CRUSHED "粉砕された"
#define MES_EFFECT_A_TREE_(MES) "木は%s。", (MES)
#define MES_EFFECT_CRUSHED2(MES) "%sが割れた！", (MES)
#define MES_EFFECT_PSY_DEAD "の精神は崩壊し、肉体は抜け殻となった。"
#define MES_EFFECT_ANTIPATHY "はあなたに敵意を抱いている。"
#define MES_EFFECT_TAMED "を支配した。"
#define MES_EFFECT_ALREADY_THRALL "は既にあなたの奴隷だ。"
#define MES_EFFECT_STASIS "は動けなくなった！"
#define MES_EFFECT_FRIENDLY "は突然友好的になったようだ！"
#define MES_EFFECT_OTHER_HEALTHIER(CREATURE) "%^sは気分が良さそうだ。", (CREATURE)->name
#define MES_EFFECT_HAND_OF_DOOM "あなたは命が薄まっていくように感じた！"
#define MES_EFFECT_PHOTO(TARGET) "%sを写真に撮った。", (TARGET)->name
#define MES_EFFECT_HEAVY_REDUCE_STAT(TARGET, STAT) "%sの%sが大きく損なわれた。", (TARGET)->name, (STAT)
#define MES_EFFECT_HEAVY_REDUCE_STAT_ALL(TARGET) "%sの能力が大きく衰えた。", (TARGET)->name
#define MES_EFFECT_GENOCIDE(TARGET) "%sは消滅した！", (TARGET)->name
#else
#define MES_EFFECT_KILL_ACID (plural ? " melt!" : " melts!")
#define MES_EFFECT_KILL_FIRE (plural ? " burn up!" : " burns up!")
#define MES_EFFECT_KILL_COLD (plural ? " shatter!" : " shatters!")
#define MES_EFFECT_KILL_DISINTEGRATE (plural ? " evaporate!" : " evaporates!")
#define MES_EFFECT_REVIVED " revived."
#define MES_EFFECT_UNEFFECTED_OBJECT "The %s %s unaffected.", object_name, (plural ? "are" : "is")
#define MES_EFFECT_KILL_OBJECT(OBJECT_NAME, KILL) "The %s%s", (OBJECT_NAME), (KILL)
#define MES_EFFECT_CHAOS_DONE "Your body is twisted by chaos!"
#define MES_EFFECT_TIME_DONE(TARGET) "You saw %s' life has clocked back.", (TARGET)
#define MES_EFFECT_GRAVITY_DONE "Gravity warps around you."
#define MES_EFFECT_MIRROR_CRUSH "The mirror was crashed!"
#define MES_EFFECT_MIND_BACKFIRE "%^s corrupted mind backlashes your attack!"
#define MES_EFFECT_DISARM_TRAP "Click!"
#define MES_EFFECT_CUT_ARROW "You cut down the arrow!"
#define MES_EFFECT_BLASTED "was blasted."
#define MES_EFFECT_SHRANK "shrank."
#define MES_EFFECT_MELTED "melted."
#define MES_EFFECT_FROZEN "was frozen and smashed."
#define MES_EFFECT_BURN_UP "burns up!"
#define MES_EFFECT_CRUSHED "was crushed."
#define MES_EFFECT_A_TREE_(MES) "A tree %s", (MES)
#define MES_EFFECT_CRUSHED2(MES) "The %s was crashed!", (MES)
#define MES_EFFECT_PSY_DEAD " collapses, a mindless husk."
#define MES_EFFECT_ANTIPATHY " hates you too much!"
#define MES_EFFECT_TAMED " is tamed!"
#define MES_EFFECT_ALREADY_THRALL " is in your thrall!"
#define MES_EFFECT_STASIS " is suspended!"
#define MES_EFFECT_FRIENDLY " suddenly seems friendly!"
#define MES_EFFECT_OTHER_HEALTHIER(CREATURE) "%^s appears healthier.", (CREATURE)->name
#define MES_EFFECT_HAND_OF_DOOM "You feel your life fade away!"
#define MES_EFFECT_PHOTO(TARGET) "You take a photograph of %s.", (TARGET)->name
#define MES_EFFECT_HEAVY_REDUCE_STAT(TARGET, STAT) "%s is not as %s as you used to be.", (TARGET)->name, (STAT)
#define MES_EFFECT_HEAVY_REDUCE_STAT_ALL(TARGET) "%s is not as powerful as you used to be...", (TARGET)->name, (STAT)
#define MES_EFFECT_GENOCIDE(TARGET) "%^s disappered!", (TARGET)->name
#endif

#ifdef JP
#define MES_BIRTH_SELECT_CAMPAIGN "キャンペーンを選択して下さい"
#define MES_BIRTH_PROFILE_LOAD_ASK "プロフィールファイルをロードしますか? "
#define MES_BIRTH_PROFILE_LOAD_FAILED "プロフィールファイルの読み込みに失敗しました。"
#define MES_BIRTH_PROFILE_LOAD_NO_DATA "有効なプロフィールはこのファイルにありません。"
#define MES_BIRTH_REALM "領域を選択して下さい:"
#define MES_BIRTH_PATRON "主神を選択して下さい:"
#define MES_BIRTH_PERSONALITY "性格を選択して下さい:"
#define MES_BIRTH_START_POINT "開始地点を選択して下さい:"
#else
#define MES_BIRTH_SELECT_CAMPAIGN "Select Campaign"
#define MES_BIRTH_PROFILE_LOAD_ASK "Load profile preference file? "
#define MES_BIRTH_PROFILE_LOAD_FAILED "Failed to load profile preference."
#define MES_BIRTH_PROFILE_LOAD_NO_DATA "There does not exist valid profile preference."
#define MES_BIRTH_REALM "Select a realm:"
#define MES_BIRTH_PATRON "Select a patron:"
#define MES_BIRTH_PERSONALITY "Select a personality:"
#define MES_BIRTH_START_POINT "Select a starting point:"
#endif

#ifdef JP
#define MES_DUNGEON_INFO(NAME, LEVEL) "%s(%d階相当)", (NAME), (LEVEL)
#else
#define MES_DUNGEON_INFO(NAME, LEVEL) "%s(level %d)", (NAME), (LEVEL)
#endif

#ifdef JP
#define MES_QUEST_INFO(LEVEL) "クエスト情報 (危険度: %d 階相当)", (LEVEL)
#define MES_QUEST_INFO2(NAME, LEVEL) "クエスト「%s」(%d階相当)", (NAME), (LEVEL)
#define MES_QUEST_NO_QUEST "今のところクエストはありません。"
#define MES_QUEST_DEFAULT_KILLING "クエスト: %sを %d体倒す"
#define MES_QUEST_WARN_UNCOMPLETE "あなたは現在のクエストを終了させていません！\nCTRL-Qを使えばクエストの状態がチェックできます。\nクエストを終わらせたら戻って来て下さい。\n"
#define MES_QUEST_ASK_ACCEPT "このクエストを受諾しますか？"
#define MES_QUEST_ACCEPT "クエスト『%s』を受諾しました。"
#define MES_QUEST_NO_ENTRANCE "ここにはクエストの入口はない"
#define MES_QUEST_ENTRANCE "ここにはクエストへの入口があります。"
#define MES_QUEST_ASK_ENTER "クエストに入りますか？"
#define MES_QUEST_TARGET_INFO(TARGET) "注意せよ！この階は%sによって守られている！", (TARGET)
#define MES_QUEST_LORD(FLOOR, TARGET) "この階には%sの主である%sが棲んでいる。", (FLOOR), (TARGET)
#define MES_QUEST_TARGETS_INFO(TARGET, NUM) "注意しろ！この階は%d体の%sによって守られている！", (NUM), (TARGET)
#define MES_QUEST_TARGET_GONE "この階は以前は誰かによって守られていたようだ…。"
#define MES_QUEST_EXIT_WARNING "この階を一度去ると二度と戻って来られません。"
#define MES_QUEST_ASK_EXIT "本当にこの階を去りますか？"
#define MES_QUEST_TYPE_KILL_ONE(FLOOR, TARGET) "「%s」にいる「%s」を倒す。", (FLOOR), (TARGET)
#define MES_QUEST_TYPE_FIND_OBJECT(FLOOR, TARGET) "「%s」にある「%s」を見つけ出す。", (FLOOR), (TARGET)
#define MES_QUEST_TYPE_SEARCH "探索する。"
#define MES_QUEST_TYPE_KILL_ALL "全てのクリーチャーを倒す。"
#define MES_QUEST_TYPE_KILL_NUMBER(MAX, CUR) "%d 体のクリーチャーを倒す。(%d 体殺害)", (MAX), (CUR)
#else
#define MES_QUEST_INFO2(NAME, LEVEL) "the entrance to the quest '%s'(level %d)", (NAME), (LEVEL)
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
#define MES_QUEST_LORD(FLOOR, TARGET) "%^s lives in this level as the keeper of %s.", (TARGET), (FLOOR)
#define MES_QUEST_TARGETS_INFO(TARGET, NUM) "Be warned, this level is guarded by %d %s!", (NUM), (TARGET)
#define MES_QUEST_TARGET_GONE "It seems that this level was protected by someone before..."
#define MES_QUEST_EXIT_WARNING "You can't come back here once you leave this floor."
#define MES_QUEST_ASK_EXIT "Really leave this floor? "
#define MES_QUEST_TYPE_KILL_ONE(FLOOR, TARGET) "kill %s in %s.", (TARGET), (FLOOR)
#define MES_QUEST_TYPE_FIND_OBJECT(FLOOR, TARGET) "Find out %s in %s.", (FLOOR), (TARGET)
#define MES_QUEST_TYPE_SEARCH "Search."
#define MES_QUEST_TYPE_KILL_ALL "Kill all creatures."
#define MES_QUEST_TYPE_KILL_NUMBER(MAX, CUR) "Kill %d creatures, have killed %d.", (MAX), (CUR)
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
#define MES_CREATURE_HP_GAIN(POINT) "最大ヒット・ポイントが %d 増加した！", (POINT)
#define MES_CREATURE_MP_GAIN(POINT) "最大マジック・ポイントが %d 増加した！", (POINT)
#define MES_CREATURE_INC_STATUS(CREATURE, INC) "ワーオ！%sはとても%sなった！", (CREATURE)->name, (INC)
#define MES_CREATURE_DEC_STATUS(CREATURE, DEC) "%sはひどく%sなった。", (CREATURE)->name, (DEC)
#define MES_CREATURE_DEC_STATUS_CANCEL(CREATURE, DEC) "%sはひどく%sなりかけたが、元に戻った", (CREATURE)->name, (DEC)
#define MES_CREATURE_RES_STATUS(CREATURE, RES) "%sは元通りに%sなった。", (CREATURE)->name, (RES)
#define MES_CREATURE_LIFE_RATE(NUM) "現在の体力ランク : %d/100", (NUM)
#define MES_CREATURE_RES_EXP "ソウルが回復したような気がする。"
#define MES_CREATURE_GAIN_EXP "更にソウルを得たような気がする。"
#define MES_CREATURE_WHICH_GAIN "どの能力値を上げますか？"
#define MES_CREATURE_KARMA(TYPE, NUM) "[%s]の業: %d\n", (TYPE), (NUM)
#else
#define MES_CREATURE_LEVELUP(LEV) "Welcome to level %d.", (LEV)
#define MES_CREATURE_HP_GAIN(POINT) "Max-HitPoint increased in %d.", (POINT)
#define MES_CREATURE_MP_GAIN(POINT) "Max-ManaPoint increased in %d.", (POINT)
#define MES_CREATURE_INC_STATUS(CREATURE, INC) "Wow! %s became very %s!", (CREATURE)->name, (INC)
#define MES_CREATURE_DEC_STATUS(CREATURE, DEC) "%s beaome very %s.", (CREATURE)->name, (DEC)
#define MES_CREATURE_DEC_STATUS_CANCEL(CREATURE, DEC) "%s become %s for a moment, but the feeling passes.", (CREATURE)->name, (DEC)
#define MES_CREATURE_RES_STATUS(CREATURE, RES) "%s became %s as before.", (CREATURE)->name, (RES)
#define MES_CREATURE_LIFE_RATE(NUM) "Your current Life Rating is %d/100.", (NUM)
#define MES_CREATURE_RES_EXP "You feel your soul energies returning."
#define MES_CREATURE_GAIN_EXP "You feel more souls."
#define MES_CREATURE_WHICH_GAIN "Which stat do you want to raise?"
#define MES_CREATURE_KARMA(TYPE, NUM) "Your karma of %s is %d.", (TYPE), (NUM)
#endif

#ifdef JP
#define MES_POLYSELF_ASK "変身します。よろしいですか？"
#define MES_POLYSELF_FEELING "あなたは変化の訪れを感じた..."
#define MES_POLYSELF_ORDINARY "奇妙なくらい普通になった気がする。"
#define MES_POLYSELF_DIFFICULT_FORM "現在の姿で生きていくのは困難なようだ！"
#define MES_POLYMORPH_DONE(CREATURE) "突然%sが変身した。", (CREATURE)
#define MES_POLYSELF_LESS_WOUND "負っていた傷がより軽くなった。"
#define MES_POLYSELF_MORE_WOUND "新たな傷ができた！"
#else
#define MES_POLYSELF_ASK "You will polymorph your self. Are you sure? "
#define MES_POLYSELF_FEELING "You feel a change coming over you..."
#define MES_POLYSELF_ORDINARY "You feel oddly normal."
#define MES_POLYSELF_DIFFICULT_FORM "You find living difficult in your present form!"
#define MES_POLYMORPH_DONE(CREATURE) "Suddenly, %s transforms!", (CREATURE)
#define MES_POLYSELF_LESS_WOUND "Your wounds are polymorphed into less serious ones."
#define MES_POLYSELF_MORE_WOUND "A new wound was created!"
#endif

#ifdef JP
#define MES_STORE_RUMOR "店主は耳うちした:"
#define MES_STORE_DECIDE_INSTANTLY "即座にこの金額にまとまった。"
#define MES_STORE_DECIDE_FINALY "結局この金額にまとまった。"
#define MES_STORE_DECIDE_QUICKLY "すんなりとこの金額にまとまった。"
#define MES_STORE_LAST_BID(PRICE) "前回の提示価格 $%ld", (PRICE)
#define MES_STORE_ASK_PRICE "提示する価格? "
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
#define MES_STORE_PAGE(NUM) "(%dページ)  ", (NUM)
#define MES_STORE_NUM(CUR, MAX) "アイテム数:  %4d/%4d", (CUR), (MAX)
#define MES_STORE_ITEM_DESCRIPTION "アイテムの一覧"
#define MES_STORE_ITEM_DESCRIPTION "アイテムの一覧"
#else
#define MES_STORE_RUMOR "The shopkeeper whispers something into your ear:"
#define MES_STORE_DECIDE_INSTANTLY "You instantly agree upon the price."
#define MES_STORE_DECIDE_FINALY "You eventually agree upon the price."
#define MES_STORE_DECIDE_QUICKLY "You quickly agree upon the price."
#define MES_STORE_LAST_BID(PRICE) "Your last bid %ld", (PRICE)
#define MES_STORE_ASK_PRICE "What price do you ask? "
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
#define MES_STORE_PAGE(NUM) "(Page%d)  ", (NUM)
#define MES_STORE_NUM(CUR, MAX) "Objects:  %4d/%4d", (CUR), (MAX)
#define MES_STORE_ITEM_DESCRIPTION "Item Description"
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
#define MES_INTERFACE_QUANTITY(NUM) "いくつですか (1-%d): ", (NUM)
#define MES_INTERFACE_DUMP "['h'でモード変更, ESCで終了]"
#define MES_INTERFACE_TO_SEE " '*'一覧,"
#define MES_INTERFACE_EQUIP_AND_INVEN "'4' 装備品, '6' 持ち物,"
#define MES_INTERFACE_INVEN " '4'or'6' 持ち物,"
#define MES_INTERFACE_EQUIP " '4'or'6' 装備品,"
#define MES_INTERFACE_ENTER_SCROLL_DOWN " Enter 次,"
#define MES_INTERFACE_CTRL "CTRL: "
#define MES_INTERFACE_OPTION "<方向>で移動, Enterで決定, ESCでキャンセル, ?でヘルプ: "
#define MES_INTERFACE_FLOOR_OBJECT " '-'床上,"
#define MES_INTERFACE_TARGET_1 "q止 t決 p自 o現 +次 -前"
#define MES_INTERFACE_TARGET_2 "q止 p自 o現 +次 -前"
#define MES_INTERFACE_TARGET_3 "q止 t決 p自 m近 +次 -前"
#define MES_INTERFACE_TARGET_4 "方向 ('*'でターゲット選択, ESCで中断)? "
#define MES_INTERFACE_TARGET_5 "方向 ('5'でターゲットへ, '*'でターゲット再選択, ESCで中断)? "
#define MES_INTERFACE_RESERACH " ['r'思い出, ' 'で続行, ESC]"
#define MES_INTERFACE_RESERACH2 "クリーチャーの文字を入力して下さい(記号 or ^A全,^Uユ,^N非ユ,^M名前):"
#define MES_INTERFACE_VISUAL "コマンド (n/N/^N/a/A/^A/c/C/^C/l/L/^L/d/D/^D/v/V/^V): "
#define MES_INTERFACE_SKILL_LIST "Lv   MP 失率 効果"
#define MES_INTERFACE_SKILL_LIST2 "  Lv   MP"
#define MES_INTERFACE_SKILL_LIST3 "熟練度 Lv   MP 失率 効果"
#define MES_INTERFACE_SNIPE_LIST "Lv   集中度"
#define MES_INTERFACE_RACIAL_LIST "                            Lv   MP 失率"
#define MES_INTERFACE_FORCE "('w'練気術, ESC) 'w'かESCを押してください。 "
#define MES_INTERFACE_FORCE2 " 'w'練気術,"
#define MES_INTERFACE_FOR_INVEN " '/' 持ち物,"
#define MES_INTERFACE_FOR_EQUIP " '/' 装備品,"
#define MES_INTERFACE_LOCATE(Y, X, DIR) "マップ位置 [%d,%d] (プレイヤーの%s)  方向?"
#define MES_INTERFACE_EDIT_HISTORY "[ カーソルキーで移動、Enterで終了、Ctrl-Aでファイル読み込み ]"
#define MES_INTERFACE_CREATURE_DUMP1 ", 'h' その他の情報"
#define MES_INTERFACE_CREATURE_DUMP2 ", Enter この数値に決定"
#define MES_INTERFACE_CREATURE_DUMP3 "[ 'Q' 中断, 'S' 初めから, Enter ゲーム開始 ]"
#define MES_INTERFACE_VIEW_MAP "何かキーを押してください('M':拾う 'N':放置 'D':M+N 'K':壊すアイテムを表示)"
#define MES_INTERFACE_DUMP_VERSION(VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH) "[%s %d.%d.%d キャラクタ情報]\n\n", (VERSION_NAME), (VER_MAJOR), (VER_MINOR), (VER_PATCH)
#define MES_INTERFACE_LEARNED_BLUEMAGIC "\n\n  [学習済みの青魔法]\n"
#define MES_INTERFACE_EATEN_MAGIC "\n\n  [取り込んだ魔法道具]\n"
#define MES_INTERFACE_QUEST_INFO "\n\n  [クエスト情報]\n"
#define MES_INTERFACE_LAST_MESSAGE "\n  [死ぬ直前のメッセージ]\n\n"
#define MES_INTERFACE_WINNING_MESSAGE "\n  [*勝利*メッセージ]\n\n"
#define MES_INTERFACE_RECALL_DEPTH "\n  [帰還場所]\n\n"
#define MES_INTERFACE_RECALL_DEPTH_LIST(CONQUESTED, NAME, DEPTH) "   %c%-12s: %3d 階\n", (CONQUESTED) ? '!' : ' ', (NAME), (DEPTH)
#define MES_INTERFACE_DEFEATED_CREATURE "\n  [倒したクリーチャー]\n\n"
#define MES_INTERFACE_DEFEATED_SCORE(UNIQUE, TOTAL) "%ld体のユニーク・クリーチャーを含む、合計%ld体の敵を倒しています。\n", (UNIQUE), (TOTAL)
#define MES_INTERFACE_DEFEATED_UNIQUE_TOP(NUM) "\n《上位%ld体のユニーク・クリーチャー》\n", (NUM)
#define MES_INTERFACE_FIRST_RACE(CREATURE) "\n\n あなたは%sとして生まれた。", desc_race_name((CREATURE), CD_SEX)
#define MES_INTERFACE_MUTATION "\n\n  [突然変異]\n\n"
#define MES_INTERFACE_CHECKSUM(SUM) "  [チェックサム: \"%s\"]\n\n", (SUM)
#else
#define MES_INTERFACE_QUANTITY(NUM) "Quantity (1-%d): ", (NUM)
#define MES_INTERFACE_DUMP "['h' to change mode, or ESC]"
#define MES_INTERFACE_TO_SEE " * to see,"
#define MES_INTERFACE_EQUIP_AND_INVEN " 4 for Equip, 6 for Inven,"
#define MES_INTERFACE_INVEN " 4 or 6 for Inven,"
#define MES_INTERFACE_EQUIP " 4 or 6 for Equip,"
#define MES_INTERFACE_ENTER_SCROLL_DOWN " Enter for scroll down,"
#define MES_INTERFACE_CTRL "Control: "
#define MES_INTERFACE_OPTION "Move to <dir>, Select to Enter, Cancel to ESC, ? to help: "
#define MES_INTERFACE_FLOOR_OBJECT " - for floor,"
#define MES_INTERFACE_TARGET_1 "q,t,p,o,+,-,<dir>"
#define MES_INTERFACE_TARGET_2 "q,p,o,+,-,<dir>"
#define MES_INTERFACE_TARGET_3 "q,t,p,m,+,-,<dir>"
#define MES_INTERFACE_TARGET_4 "Direction ('*' to choose a target, Escape to cancel)? "
#define MES_INTERFACE_TARGET_5 "Direction ('5' for target, '*' to re-target, Escape to cancel)? "
#define MES_INTERFACE_RESERACH " [(r)ecall, ESC, space to continue]"
#define MES_INTERFACE_RESERACH2 "Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): "
#define MES_INTERFACE_VISUAL "Command (n/N/^N/a/A/^A/c/C/^C/l/L/^L/d/D/^D/v/V/^V): "
#define MES_INTERFACE_SKILL_LIST "Lv Mana Fail Info"
#define MES_INTERFACE_SKILL_LIST2 "  Lv Mana"
#define MES_INTERFACE_SKILL_LIST3 "Profic Lv   SP Fail Effect"
#define MES_INTERFACE_SNIPE_LIST "Lv Pow"
#define MES_INTERFACE_RACIAL_LIST "                            Lv Cost Fail"
#define MES_INTERFACE_FORCE "(w for the Force, ESC) Hit 'w' or ESC. "
#define MES_INTERFACE_FORCE2 " w for the Force,"
#define MES_INTERFACE_FOR_INVEN " / for Inven,"
#define MES_INTERFACE_FOR_EQUIP " / for Equip,"
#define MES_INTERFACE_LOCATE(Y, X, DIR) "Map sector [%d,%d], which is %s your sector.  Direction?"
#define MES_INTERFACE_EDIT_HISTORY "[ Cursor key for Move, Enter for End, Ctrl-A for Read pref ]"
#define MES_INTERFACE_CREATURE_DUMP1 ", 'h' Misc."
#define MES_INTERFACE_CREATURE_DUMP2 ", or Enter to accept"
#define MES_INTERFACE_CREATURE_DUMP3 "['Q'uit, 'S'tart over, or Enter to continue]"
#define MES_INTERFACE_VIEW_MAP " Hit M, N(for ~), K(for !), or D(same as M+N) to display auto-picker items."
#define MES_INTERFACE_DUMP_VERSION(VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH) "  [%s %d.%d.%d Character Dump]\n\n", (VERSION_NAME), (VER_MAJOR), (VER_MINOR), (VER_PATCH)
#define MES_INTERFACE_LEARNED_BLUEMAGIC "\n\n  [Learned Blue Magic]\n"
#define MES_INTERFACE_EATEN_MAGIC "\n\n  [Magic devices eaten]\n"
#define MES_INTERFACE_QUEST_INFO "\n\n  [Quest Information]\n"
#define MES_INTERFACE_LAST_MESSAGE "\n  [Last Messages]\n\n"
#define MES_INTERFACE_WINNING_MESSAGE "\n  [*Winning* Message]\n\n"
#define MES_INTERFACE_RECALL_DEPTH "\n  [Recall Depth]\n\n"
#define MES_INTERFACE_RECALL_DEPTH_LIST(CONQUESTED, NAME, DEPTH) "   %c%-16s: level %3d\n", (CONQUESTED) ? '!' : ' ', (NAME), (DEPTH)
#define MES_INTERFACE_DEFEATED_CREATURE "\n  [Defeated Creatures]\n\n"
#define MES_INTERFACE_DEFEATED_SCORE(UNIQUE, TOTAL) "You have defeated %ld %s including %ld unique creature%s in total.\n", (TOTAL), (TOTAL) == 1 ? "enemy" : "enemies",(UNIQUE), ((UNIQUE) == 1 ? "" : "s"))
#define MES_INTERFACE_DEFEATED_UNIQUE_TOP(NUM) "\n< Unique creatures top %ld >\n", (NUM)
#define MES_INTERFACE_FIRST_RACE(CREATURE) "\n\n You were born as %s.", desc_race_name((CREATURE), CD_SEX)
#define MES_INTERFACE_MUTATION "\n\n  [Mutation]\n\n"
#define MES_INTERFACE_CHECKSUM(SUM) "  [Check Sum: \"%s\"]\n\n", (SUM)
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
#define MES_MELEE_RESELECT "攻撃を再選択しました。"
#define MES_MELEE_EYE_EYE(ATTACKER) "攻撃が%s自身を傷つけた！", (ATTACKER)->name
#define MES_MELEE_JOJ_DONE "審判の宝石は赤く明るく光った！"
#define MES_MELEE_JOJ_STRAIN "宝石はあなたの体力を奪った..."
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
#define MES_MELEE_RESELECT "Attack re-selected."
#define MES_MELEE_EYE_EYE(ATTACKER) "The attack of %s has wounded %s!", (ATTACKER)->name, (ATTACKER)->name /*TODO*/
#define MES_MELEE_JOJ_DONE "The Jewel flashes bright red!"
#define MES_MELEE_JOJ_STRAIN "The Jewel drains your vitality..."
#endif

#ifdef JP
#define MES_SHOOT_ARTIFACT_ARROW(ARROW, TARGET) "%sは%sに突き刺さった！", (ARROW)->name, (TARGET)->name
#define MES_SHOOT_MARK(ARROW) "%sが敵を捕捉した。", (ARROW)
#define MES_SHOOT_ROCK_SHATTER "岩が砕け散った。"
#else
#define MES_SHOOT_ARTIFACT_ARROW(ARROW, TARGET) "%^s have stuck into %s!", (ARROW)->name, (TARGET)->name
#define MES_SHOOT_MARK(ARROW) "The %s finds a mark.", (ARROW)
#define MES_SHOOT_ROCK_SHATTER "Wall rocks were shattered."
#endif

#ifdef JP
#define MES_THROW_FIGURE_FAILED "人形は捻じ曲がり砕け散ってしまった！"
#define MES_THROW_FIGURE_CURSED "これはあまり良くない気がする。"
#define MES_THROW_SHATTER(OBJECT) "%sは砕け散った！", (OBJECT)->name
#define MES_THROW_BACK(OBJECT) "%sが手元に返ってきた。", (OBJECT)->name
#define MES_THROW_NO_BACK(OBJECT) "%sが返ってこなかった！", (OBJECT)->name
#define MES_THROW_BACK_FAILED(OBJECT) "%sを受け損ねた！", (OBJECT)->name
#define MES_THROW_BACK_GONE(OBJECT) "%sはどこかへ行った。", (OBJECT)->name
#else
#define MES_THROW_FIGURE_FAILED "The Figurine writhes and then shatters."
#define MES_THROW_FIGURE_CURSED "You have a bad feeling about this."
#define MES_THROW_SHATTER(OBJECT) "The %s shatters!", (OBJECT)->name
#define MES_THROW_BACK(OBJECT) "%s comes back to you.", (OBJECT)->name
#define MES_THROW_NO_BACK(OBJECT) , (OBJECT)->name
#define MES_THROW_BACK_FAILED(OBJECT) "%s backs, but you can't catch!", (OBJECT)->name
#define MES_THROW_BACK_GONE(OBJECT) "The %s have gone to somewhere.", (OBJECT)->name
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
#define MES_STUDY_SLOT(NUM, REALM) "あと %d 種の%sを学べる。", (NUM), (REALM)
#define MES_STUDY_NO_SPELL(REALM) "その本には学ぶべき%sがない。", (REALM)
#define MES_STUDY_PERFECT_LEARNED(REALM) "その%sは完全に使いこなせるので学ぶ必要はない。", (REALM)
#define MES_SPELL_CHANGE_REALM_ASK "本当に魔法の領域を変更しますか？"
#define MES_SPELL_FAILED "呪文をうまく唱えられなかった！"
#define MES_SPELL_CANNOT "呪文を唱えられない！"
#define MES_SPELL_NO_MORE_SPELLING "これ以上新しい呪文を詠唱することはできない。"
#define MES_SPELL_ALREADY "その呪文はすでに詠唱中だ。"
#define MES_SPELL_FORGET(REALM, SPELL) "%sの%sを忘れてしまった。", (REALM), (SPELL)
#define MES_SPELL_REMEMBER(REALM, SPELL) "%sの%sを思い出した。", (REALM), (SPELL)
#else
#define MES_STUDY_NO_READING "You cannot read books!"
#define MES_STUDY_NO_SLOT(REALM) "You cannot learn any new %ss!", (REALM)
#define MES_STUDY_SLOT(REALM, NUM) "You can learn %d new %s%s.", (REALM), (NUM), ((NUM) == 1 ? "":"s")
#define MES_STUDY_NO_SPELL(REALM) "You cannot learn any %ss in that book.", (REALM)
#define MES_STUDY_PERFECT_LEARNED(REALM) "You don't need to study this %s anymore.", (REALM)
#define MES_SPELL_CHANGE_REALM_ASK "Really, change magic realm? "
#define MES_SPELL_FAILED "You failed to get the magic off!"
#define MES_SPELL_CANNOT "You cannot cast spells!"
#define MES_SPELL_NO_MORE_SPELLING "Can not spell new spells more."
#define MES_SPELL_ALREADY "You are already casting it."
#define MES_SPELL_FORGET(REALM, SPELL) "You have forgotten the %s of %s.", (SPELL), (REALM)
#define MES_SPELL_REMEMBER(REALM, SPELL) "You have remembered the %s of %s.", (SPELL), (REALM)
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
#define MES_AUTOPICK_COMMANDKEY "C:%d:<コマンドキー>: "
#define MES_AUTOPICK_WHICH_ENTRY "どのアイテムを登録しますか"
#define MES_AUTOPICK_NO_ENTRY "アイテムを持っていない。"
#define MES_AUTOPICK_WHICH_SEARCH "どのアイテムを検索しますか? "
#define MES_AUTOPICK_NO_SEARCH "アイテムを持っていない。"
#define MES_AUTOPICK_LOAD_FILE "自動拾い設定ファイルをロードしますか? "
#define MES_AUTOPICK_AUTODUMP "\n# 自動マクロセーブ\n\n"
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
#define MES_AUTOPICK_DISCARD_QUIT "全ての変更を破棄してから終了します。よろしいですか？ "
#define MES_AUTOPICK_DISCARD_REVERT "全ての変更を破棄して元の状態に戻します。よろしいですか？ "
#define MES_AUTOPICK_EDITOR "(^Q:終了 ^W:セーブして終了, ESC:メニュー, その他:入力)"
#define MES_AUTOPICK_TRIGGER_KEY "P:<トリガーキー>: "
#else
#define MES_AUTOPICK_COMMANDKEY "<Keypress>"
#define MES_AUTOPICK_WHICH_ENTRY "Enter which item? "
#define MES_AUTOPICK_NO_ENTRY "You have nothing to enter."
#define MES_AUTOPICK_WHICH_SEARCH "Search which item? "
#define MES_AUTOPICK_NO_SEARCH "You have nothing to search."
#define MES_AUTOPICK_LOAD_FILE "Reload auto-pick preference file? "
#define MES_AUTOPICK_AUTODUMP "\n# Automatic macro dump\n\n"
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
#define MES_AUTOPICK_DISCARD_QUIT "Discard all changes and quit. Are you sure? "
#define MES_AUTOPICK_DISCARD_REVERT "Discard all changes and revert to original file. Are you sure? "
#define MES_AUTOPICK_EDITOR "(^Q:Quit, ^W:Save&Quit, ESC:Menu, Other:Input text)"
#define MES_AUTOPICK_TRIGGER_KEY "P:<Trigger key>: "
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
#define MES_RECHARGE_WHICH_ITEM "どのアイテムに魔力を充填しますか? "
#define MES_RECHARGE_NO_ITEM "魔力を充填すべきアイテムがない。"
#define MES_RECHARGE_NO_NEED "それは再充填する必要はありません。"
#define MES_RECHARGE_NO_MONEY(NAME, MONEY) "%sを再充填するには＄%d 必要です！", (NAME), (MONEY)
#define MES_RECHARGE_NO_MONEY_ALL(MONEY) "すべてのアイテムを再充填するには＄%d 必要です！", (MONEY)
#define MES_RECHARGE_ALL_ASK(MONEY) "すべてのアイテムを ＄%d で再充填しますか？", (MONEY)
#define MES_RECHARGED(NAME, MONEY, NUM) "%sを＄%d で再充填しました。", (NAME), (MONEY), (NUM)
#define MES_RECHARGED_ALL(MONEY) "＄%d で再充填しました。", (MONEY)
#define MES_RECHARGE_FAILED(NAME) "魔力が逆流した！%sは完全に魔力を失った。", (NAME)
#define MES_RECHARGE_FAILED2 "魔力が逆噴射して、ロッドからさらに魔力を吸い取ってしまった！"
#define MES_RECHARGE_FAILED3(NAME) "%sは破損を免れたが、魔力が全て失われた。", (NAME)
#define MES_RECHARGE_BROKEN1(NAME) "乱暴な魔法のために%sが壊れた！", (NAME)
#define MES_RECHARGE_BROKEN2(NAME) "乱暴な魔法のために%sが一本壊れた！", (NAME)
#define MES_RECHARGE_BROKEN3(NAME) "乱暴な魔法のために%sが全て壊れた！", (NAME)
#define MES_RECHARGE_COMMENT "再充填の費用はアイテムの種類によります。"
#define MES_RECHARGE_COUNT(MONEY) "一回分＄%d で何回分充填しますか？", (MONEY)
#define MES_RECHARGE_ROD_PRICE(NUM, MONEY) "ロッド%d本を＄%d で再充填しますか？", (NUM), (MONEY)
#define MES_RECHARGE_NEED_IDENTIFY "充填する前に鑑定されている必要があります。"
#define MES_RECHARGE_ASK_IDENTIFY(MONEY) "＄%dで鑑定しますか？ ", (MONEY)
#define MES_RECHARGE_INDENTIFIED(OBJECT) "%s です。", (OBJECT)->name
#else
#define MES_RECHARGE_WHICH_ITEM "Recharge which item? "
#define MES_RECHARGE_NO_ITEM "You have nothing to recharge."
#define MES_RECHARGE_NO_NEED "That doesn't need to be recharged."
#define MES_RECHARGE_NO_MONEY(NAME, MONEY) "You need %d gold to recharge %s!", (MONEY), (NAME)
#define MES_RECHARGE_NO_MONEY_ALL(MONEY) "You need %d gold to recharge all items!", (MONEY)
#define MES_RECHARGE_ALL_ASK(MONEY) "Recharge all items for %d gold? ", (MONEY)
#define MES_RECHARGED(NAME, MONEY, NUM) "%^s %s recharged for %d gold.", (NAME), ((NUM)> 1) ? "were" : "was"), (MONEY) 
#define MES_RECHARGED_ALL(MONEY) "You pay %d gold.", (MONEY)
#define MES_RECHARGE_FAILED(NAME) "The recharging backfires - %s is completely drained!", (NAME)
#define MES_RECHARGE_FAILED2 "The recharge backfires, draining the rod further!"
#define MES_RECHARGE_FAILED3(NAME) "You save your %s from destruction, but all charges are lost.", (NAME)
#define MES_RECHARGE_BROKEN1(NAME) "Wild magic consumes your %s!", (NAME)
#define MES_RECHARGE_BROKEN2(NAME) "Wild magic consumes one of your %s!", (NAME)
#define MES_RECHARGE_BROKEN3(NAME) "Wild magic consumes all your %s!", (NAME)
#define MES_RECHARGE_COMMENT "The prices of recharge depend on the type."
#define MES_RECHARGE_COUNT(MONEY) "Add how many charges for %d gold? ", (MONEY)
#define MES_RECHARGE_ROD_PRICE(NUM, MONEY) "Recharge the %s for %d gold? ", (((NUM) > 1) ? "rods" : "rod"), (MONEY)), 
#define MES_RECHARGE_NEED_IDENTIFY "The item must be identified first。"
#define MES_RECHARGE_ASK_IDENTIFY(MONEY) "Identify for %d gold? ", (MONEY)
#define MES_RECHARGE_INDENTIFIED(OBJECT) "You have: %s.", (OBJECT)->name
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
#define MES_INSANITY_NIGHTMARE_PRE "恐ろしい光景が心をよぎった。"
#define MES_INSANITY_NIGHTMARE(ELDRITCH) "夢の中で%sに追いかけられた。", (ELDRITCH)
#define MES_INSANITY_FACED(ADJ, ELDRITCH) "%s%sの顔を見てしまった！", (ADJ), (ELDRITCH)
#define MES_INSANITY_GLANCE(ADJ, ELDRITCH) "%s%sの顔が垣間見えた。", (ADJ), (ELDRITCH)
#define MES_INSANITY_AMNESIA "あまりの恐怖に全てのことを忘れてしまった！"

#else
#define MES_INSANITY_NIGHTMARE_PRE "Horrible visions flit through your mind."
#define MES_INSANITY_NIGHTMARE(ELDRITCH) "%^s chases you through your dreams.", (ELDRITCH)
#define MES_INSANITY_FACED(ADJ, ELDRITCH) "You behold the %s visage of %s.", (ADJ), (ELDRITCH)
#define MES_INSANITY_GLANCE(ADJ, ELDRITCH) "You glance at the %s visage of %s.", (ADJ), (ELDRITCH)
#define MES_INSANITY_AMNESIA "You forget everything in your utmost terror!"
#endif

#ifdef JP
#define MES_WALK_NEVER_MOVE "動けない。"
#define MES_WALK_NEVER_SWIM "泳げない。"
#define MES_WALK_NEVER_LAND "陸に上がれない。"
#define MES_WALK_STEED_FEAR(STEED) "%sが恐怖していて制御できない。", (STEED)
#define MES_WALK_STEED_STUNED(STEED) "%sが朦朧としていてうまく動けない！", (STEED)
#define MES_WALK_TOO_HOT(FEATURE) "熱くて%sの上に行けない。", (FEATURE)
#define MES_WALK_MUST_FLY(FEATURE) "空を飛ばないと%sの上には行けない。", (FEATURE)
#define MES_WALK_BLOCK(FEATURE) "%sが行く手をはばんでいる。", (FEATURE)
#define MES_WALK_ASK_ENTERING_CHAOS "本当に〈混沌〉の領域に入りますか？"
#define MES_WALK_PUSH_PAST(TARGET) "%sを押し退けた。", (TARGET)
#define MES_WALK_CANNOT_RUN "その方向には走れません。"
#define MES_WALK_NO_ROUTE "道筋が見つかりません！"
#else
#define MES_WALK_NEVER_MOVE "Can't move."
#define MES_WALK_NEVER_SWIM "Can't swim"
#define MES_WALK_NEVER_LAND "Can't land"
#define MES_WALK_STEED_FEAR(STEED) "%^s is too scared to control.", (STEED)
#define MES_WALK_STEED_STUNED(STEED) "You cannot control stunned %s!", (STEED)
#define MES_WALK_TOO_HOT(FEATURE) "Too hot to go through %s.", (FEATURE)
#define MES_WALK_MUST_FLY(FEATURE) "You need to fly to go through the %s.", (FEATURE)
#define MES_WALK_BLOCK(FEATURE) "There is %s %s blocking your way.", is_a_vowel((FEATURE)[0]) ? "an" : "a", (FEATURE)
#define MES_WALK_ASK_ENTERING_CHAOS "Really want to enter territory of chaos? "
#define MES_WALK_PUSH_PAST(TARGET) "You push past %s.", (TARGET)
#define MES_WALK_CANNOT_RUN "You cannot run in that direction."
#define MES_WALK_NO_ROUTE "No route is found!"
#endif

#ifdef JP
#define MES_OBJECT_WHICH_IMPROVE "どのアイテムを改良しますか？"
#define MES_OBJECT_NO_IMPROVE "改良できるものがありません。"
#define MES_OBJECT_FOUND(NUM) "%d種のアイテムがある。", (NUM)
#define MES_OBJECT_WHICH_USE "どれを使いますか？"
#define MES_OBJECT_NO_USE "使えるものがありません。"
#define MES_OBJECT_WHICH_DROP "どのアイテムを落としますか? "
#define MES_OBJECT_NO_DROP "落とせるアイテムを持っていない。"
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
#define MES_OBJECT_DROP_FEET "何かが足下に転がってきた。"
#define MES_OBJECT_CHARGE_LEFT(NUM) "あと %d 回分の魔力が残っている。", (NUM)
#define MES_OBJECT_CHARGE_LEFT2(NUM) "この床上のアイテムは、あと %d 回分の魔力が残っている。", (NUM)
#define MES_OBJECT_NO_CHARGE_LEFT "もう魔力が残っていない。"
#define MES_OBJECT_WHICH_REFILL_LAMP "どの油つぼから注ぎますか? "
#define MES_OBJECT_NO_REFILL_LAMP "油つぼがない。"
#define MES_OBJECT_WHICH_TORCH "どの松明で明かりを強めますか? "
#define MES_OBJECT_NO_TORCH "他に松明がない。"
#define MES_OBJECT_DROPPED(NAME, SYM) "%s(%c)を落とした。", (NAME), (SYM)
#define MES_OBJECT_INDETIFY_ALL_ASK "持ち物を全て鑑定してよろしいですか？"
#define MES_OBJECT_INDETIFIED_ALL "持ち物全てが鑑定されました。"
#define MES_OBJECT_PICKED(CREATURE, OBJECT) "%^sが%sを拾った。", (CREATURE), (OBJECT)
#define MES_OBJECT_PICK_FAILED(CREATURE, OBJECT) "%^sは%sを拾おうとしたが、だめだった。", (CREATURE), (OBJECT)
#define MES_OBJECT_DESTORY(CREATURE, OBJECT) "%^sが%sを破壊した。", (CREATURE), (OBJECT)
#define MES_OBJECT_WHICH_PULISH "どの盾を磨きますか？"
#define MES_OBJECT_NO_PULISH "磨く盾がありません。"
#define MES_OBJECT_PULISHED(OBJECT) "%sは輝いた！", (OBJECT)->name
#else
#define MES_OBJECT_WHICH_IMPROVE "Improve which item? "
#define MES_OBJECT_NO_IMPROVE "You have nothing to improve."
#define MES_OBJECT_FOUND(NUM) "You see %d items.", (NUM)
#define MES_OBJECT_WHICH_USE "Use which item? "
#define MES_OBJECT_NO_USE "You have nothing to use." 
#define MES_OBJECT_WHICH_DROP "Drop which item? "
#define MES_OBJECT_NO_DROP "You have nothing to drop."
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
#define MES_OBJECT_DROP_FEET "You feel something roll beneath your feet."
#define MES_OBJECT_CHARGE_LEFT(NUM) "You have %d charges remaining.", (NUM)
#define MES_OBJECT_CHARGE_LEFT2(NUM) "There is %d charge remaining.", (NUM)
#define MES_OBJECT_NO_CHARGE_LEFT "No charges left."
#define MES_OBJECT_WHICH_REFILL_LAMP "Refill with which flask? "
#define MES_OBJECT_NO_REFILL_LAMP "You have no flasks of oil."
#define MES_OBJECT_WHICH_TORCH "Refuel with which torch? "
#define MES_OBJECT_NO_TORCH "You have no extra torches."
#define MES_OBJECT_DROPPED(NAME, SYM) "You drop %s (%c).", (NAME), (SYM)
#define MES_OBJECT_INDETIFY_ALL_ASK "Do you pay for identify all your possession? "
#define MES_OBJECT_INDETIFIED_ALL "Your possessions have been identified."
#define MES_OBJECT_PICK_FAILED(CREATURE, OBJECT) "%^s tries to pick up %s, but fails.", (CREATURE), (OBJECT)
#define MES_OBJECT_PICKED(CREATURE, OBJECT) "%^s picks up %s.", (CREATURE), (OBJECT)
#define MES_OBJECT_DESTORY(CREATURE, OBJECT) "%^s destroys %s.", (CREATURE), (OBJECT)
#define MES_OBJECT_WHICH_PULISH "Pulish which weapon? "
#define MES_OBJECT_NO_PULISH "You have weapon to pulish."
#define MES_OBJECT_PULISHED(OBJECT) "The %s shine%s!", (OBJECT)->name, ((OBJECT)->number > 1) ? "" : "s")
#endif

#ifdef JP
#define MES_OBJECT_WHICH_STAFF "どの杖を使いますか? "
#define MES_OBJECT_NO_STAFF "使える杖がない。"
#define MES_OBJECT_WHICH_WAND "どの魔法棒で狙いますか? "
#define MES_OBJECT_NO_WAND "使える魔法棒がない。" 
#define MES_OBJECT_WHICH_ROD "どのロッドを振りますか? "
#define MES_OBJECT_NO_ROD "使えるロッドがない。"
#define MES_OBJECT_WHICH_ACTIVATE "どのアイテムを始動させますか? "
#define MES_OBJECT_NO_ACTIVATE "始動できるアイテムを装備していない。"
#define MES_OBJECT_WAND_NEED_PICKUP "まずは魔法棒を拾わなければ。"
#define MES_OBJECT_ROD_NEED_PICKUP "まずはロッドを拾わなければ。"
#define MES_OBJECT_STAFF_FAILED "うまく杖を使えなかった。"
#define MES_OBJECT_WAND_FAILED "うまく魔法棒を使えなかった。"
#define MES_OBJECT_ROD_FAILED "うまくロッドを使えなかった。"
#define MES_OBJECT_STILL_CHARGING1 "このロッドはまだ魔力を充填している最中だ。"
#define MES_OBJECT_STILL_CHARGING2 "そのロッドはまだ充填中です。"
#define MES_OBJECT_MBALL_FAILED "解放に失敗した。"
#define MES_OBJECT_ACTIVATE_NONE "このアイテムは始動できない。"
#define MES_OBJECT_WHICH_THROW "何を投げますか? "
#define MES_OBJECT_NO_THROW "投げるアイテムがない。"
#define MES_OBJECT_WHICH_FIRE "どれを撃ちますか? "
#define MES_OBJECT_NO_FIRE "発射されるアイテムがありません。"
#else
#define MES_OBJECT_WHICH_STAFF "Use which staff? "
#define MES_OBJECT_NO_STAFF "You have no staff to use."
#define MES_OBJECT_WHICH_WAND "Aim which wand? "
#define MES_OBJECT_NO_WAND "You have no wand to aim."
#define MES_OBJECT_WHICH_ROD "Zap which rod? "
#define MES_OBJECT_NO_ROD "You have no rod to zap."
#define MES_OBJECT_WHICH_ACTIVATE "Activate which item? "
#define MES_OBJECT_NO_ACTIVATE "You have nothing to activate."
#define MES_OBJECT_WAND_NEED_PICKUP "You must first pick up the wands."
#define MES_OBJECT_ROD_NEED_PICKUP "You must first pick up the rods."
#define MES_OBJECT_STAFF_FAILED "You failed to use the staff properly."
#define MES_OBJECT_WAND_FAILED "You failed to use the wand properly."
#define MES_OBJECT_ROD_FAILED "You failed to use the rod properly."
#define MES_OBJECT_STILL_CHARGING1 "The rod is still charging."
#define MES_OBJECT_STILL_CHARGING2 "The rods are still charging."
#define MES_OBJECT_MBALL_FAILED "You failed to release your pet."
#define MES_OBJECT_ACTIVATE_NONE "That object cannot be activated."
#define MES_OBJECT_WHICH_THROW "Throw which item? "
#define MES_OBJECT_NO_THROW "You have nothing to throw."
#define MES_OBJECT_WHICH_FIRE "Fire which item? "
#define MES_OBJECT_NO_FIRE "You have nothing to fire."
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
#define MES_EQUIP_WHICH_WIELD "どれを装備しますか? "
#define MES_EQUIP_NO_WIELD "装備可能なアイテムがない。"
#define MES_EQUIP_WHICH_SLOT "どこに装備しますか?"
#define MES_EQUIP_NO_SLOT "それは装備できない。"
#define MES_EQUIP_WHICH_TAKEOFF "どれを装備からはずしますか? "
#define MES_EQUIP_NO_TAKEOFF "はずせる装備がない。"
#define MES_EQUIP_ASK_CURSED_WIELD(NAME, CURSED) "本当に%s{%s}を使いますか？", (NAME), (CURSED)
#define MES_EQUIP_ASK_BECOME_VAMPIRE(NAME) "%sを装備すると真性の吸血鬼になります。よろしいですか？", (NAME)
#define MES_EQUIP_DONE(NAME, SYM) "%s(%c)を装備した。", (NAME), (SYM)
#define MES_EQUIP_CURSED "うわ！ すさまじく冷たい！"
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
#define MES_EQUIP_BOTH_HAND(OBJECT) "%sを両手で構えた。", (OBJECT)->name
#else
#define MES_EQUIP_WHICH_WIELD "Wear/Wield which item? "
#define MES_EQUIP_NO_WIELD "You have nothing you can wear or wield."
#define MES_EQUIP_WHICH_SLOT "Equip which hand? "
#define MES_EQUIP_NO_SLOT "You can't equip it"
#define MES_EQUIP_WHICH_TAKEOFF "Take off which item? "
#define MES_EQUIP_NO_TAKEOFF "You are not wearing anything to take off."
#define MES_EQUIP_ASK_CURSED_WIELD(NAME, CURSED) "Really use the %s {%s}? ", (NAME), (CURSED)
#define MES_EQUIP_ASK_BECOME_VAMPIRE(NAME) "%s will transforms you into a true vampire permanently when equiped. Do you become a vampire?", (NAME)
#define MES_EQUIP_DONE(NAME, SYM) "You are wearing %s (%c).", (NAME), (SYM)
#define MES_EQUIP_CURSED "Oops! It feels deathly cold!"
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
#define MES_EQUIP_BOTH_HAND(OBJECT) "You are wielding %s with both hands.", (OBJECT)->name
#endif

#ifdef JP
#define MES_DAMAGE_RES1 "にはかなりの耐性がある。"
#define MES_DAMAGE_RES2 "には耐性がある。"
#define MES_DAMAGE_RES3 "にはわずかに耐性がある。"
#define MES_DAMAGE_RES4 "はひどい痛手を受けた。"
#define MES_DAMAGE_HURT_LITE "光で肉体が焦がされた！"
#define MES_DAMAGE_RES_TIME(CREATURE) "%sは時間が通り過ぎていく様子を感じた。", (CREATURE)
#define MES_DAMAGE_VOID_INVULN "バリアを切り裂いた！"
#define MES_DAMAGE_MULTI_SHADOW_SUCCESS "攻撃は幻影に命中し、届かなかった。"
#define MES_DAMAGE_MULTI_SHADOW_FAILED "幻影もろとも体が切り裂かれた！"
#define MES_DAMAGE_VOID_WRAITH_FORM "半物質の体が切り裂かれた！"
#define MES_DAMAGE_PSYONIC "霊的エネルギーで精神が攻撃された。"
#else
#define MES_DAMAGE_RES1 " resists strongly."
#define MES_DAMAGE_RES2 " resists."
#define MES_DAMAGE_RES3 " resists a little."
#define MES_DAMAGE_RES4 " is hit hard."
#define MES_DAMAGE_HURT_LITE "The light scorches your flesh!"
#define MES_DAMAGE_RES_TIME(CREATURE) "%s felt as if time is passing %s by.", (CREATURE), (CREATURE)
#define MES_DAMAGE_VOID_INVULN "The barrier is penetrated!"
#define MES_DAMAGE_MULTI_SHADOW_SUCCESS "The attack hits Shadow, unharmed."
#define MES_DAMAGE_MULTI_SHADOW_FAILED "The attack hits Shadow together with you!"
#define MES_DAMAGE_VOID_WRAITH_FORM "The attack cuts through your ethereal body!"
#define MES_DAMAGE_PSYONIC "Your mind is blasted by psyonic energy."
#endif

#ifdef JP
#define MES_DISARM_DONE(TRAP) "%sを解除した。", (TRAP)
#define MES_DISARM_CHEST "箱に仕掛けられていたトラップを解除した。"
#define MES_DISARM_NONE "そこには解除するものが見当たらない。"
#define MES_DISARM_NO_TRAP "トラップが見あたらない。"
#define MES_DISARM_NO_TRAP_CHEST "箱にはトラップが仕掛けられていない。"
#define MES_DISARM_FAILED(TARGET) "%sの解除に失敗した。", (TARGET)
#define MES_DISARM_FUMBLE "トラップを作動させてしまった！"
#else
#define MES_DISARM_DONE(TRAP) "You have disarmed the %s.", (TRAP)
#define MES_DISARM_CHEST "You have disarmed the chest."
#define MES_DISARM_NONE "You see nothing there to disarm."
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
#define MES_KNOW_ALIVE_UNIQUE_LIST1(NUM) "     地上  生存: %3d体\n", (NUM)
#define MES_KNOW_ALIVE_UNIQUE_LIST2(FROM, TO, NUM) "%3d-%3d階  生存: %3d体\n", (FROM), (TO), (NUM)
#define MES_KNOW_ALIVE_UNIQUE_LIST3(TO, NUM) "%s-   階  生存: %3d体\n", (TO), (NUM)
#else
#define MES_KNOW_PET "Current Pets"
#define MES_KNOW_ALIVE_UNIQUES "Alive Uniques"
#define MES_KNOW_NO_ALIVE_UNIQUES "No known uniques alive.\n"
#define MES_KNOW_NO_KILLED "You have killed no creatures yet.\n\n"
#define MES_KNOW_KILLED(NUMBER) "You have killed %ld %s.\n\n", (NUMBER), ((NUMBER) == 1) ? "creature" : "creatures"
#define MES_KNOW_ALIVE_UNIQUE_LIST1(NUM) "      Surface  alive: %3d\n", (NUM)
#define MES_KNOW_ALIVE_UNIQUE_LIST2(FROM, TO, NUM) "Level %3d-%3d  alive: %3d\n", (FROM), (TO), (NUM)
#define MES_KNOW_ALIVE_UNIQUE_LIST3(TO, NUM) "Level %3d-     alive: %3d\n", (TO), (NUM)
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
#define MES_EARTHQUAKE_CANCELED "ダンジョンが揺れた。"
#define MES_CALL_OF_VOID_EXPRO1 "あなたは壁に近すぎる場所で発動してしまった！"
#define MES_CALL_OF_VOID_EXPRO2 "大きな爆発音があった！"
#define MES_VANISH_DUNGEON_CANCELED "ダンジョンは一瞬静まり返った。" 
#else
#define MES_EARTHQUAKE_DONE1 "The cave ceiling collapses!"
#define MES_EARTHQUAKE_DONE2 "The cave floor twists in an unnatural way!"
#define MES_EARTHQUAKE_DONE3 "The cave quakes!  You are pummeled with debris!"
#define MES_EARTHQUAKE_HEAVY_CRUSHED "You are severely crushed!"
#define MES_EARTHQUAKE_EVADE "You nimbly dodge the blast!"
#define MES_EARTHQUAKE_CRUSHED1 "You are bashed by rubble!"
#define MES_EARTHQUAKE_CRUSHED2 "You are crushed between the floor and ceiling!"
#define MES_EARTHQUAKE_CANCELED "The dungeon trembles."
#define MES_CALL_OF_VOID_EXPRO1 "You evoked the void too close to a wall!"
#define MES_CALL_OF_VOID_EXPRO2 "There is a loud explosion!"
#define MES_VANISH_DUNGEON_CANCELED "The dungeon silences a moment."
#endif

#ifdef JP
#define MES_DIS_MINION_ASK "本当に全ペットを爆破しますか？"
#define MES_DIS_MINION_CANCEL(MINION) "%sは爆破されるのを嫌がり、勝手に自分の世界へと帰った。", (MINION)
#else
#define MES_DIS_MINION_ASK "You will blast all pets. Are you sure? "
#define MES_DIS_MINION_CANCEL(MINION)  "%^s resists to be blasted, and run away.", (MINION)
#endif

#ifdef JP
#define MES_PATRON_BOOM_OUT(PATRON) "%sの声が響き渡った:", (PATRON)
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
#define MES_PATRON_WRATH "「死ぬがよい、下僕よ！」"
#define MES_PATRON_IGNORE(PATRON) "%sはあなたを無視した。", (PATRON)
#define MES_PATRON_DEMON_SERVANT(PATRON) "%sは褒美として悪魔の使いをよこした！", (PATRON)
#define MES_PATRON_UNDEAD_SERVANT(PATRON) "%sは褒美としてアンデッドの使いをよこした！", (PATRON)
#define MES_PATRON_SERVANT(PATRON) "%sは褒美として使いをよこした！", (PATRON)
#define MES_PATRON_PRAISE_WINNER "『よくやった、我がしもべよ！』"
#define MES_PATRON_ERROR_REWARD1(PATRON) "%sの声がどもった:", (PATRON)
#define MES_PATRON_ERROR_REWARD2(TYPE, EFFECT) "「あー、あー、答えは %d/%d。文句はあるかい？」", (TYPE), (EFFECT)
#define MES_DIARY_PATRON_DESTROYED(OBJECT) "%sが破壊された。", (OBJECT)
#define MES_DIARY_PATRON_MUTATION "変異した。"
#define MES_DIARY_PATRON_GAIN_EXP "経験値を得た"
#define MES_DIARY_PATRON_LOST_EXP "経験値を失った。"
#define MES_DIARY_PATRON_GOOD_ITEM "上質なアイテムを手に入れた。"
#define MES_DIARY_PATRON_GREAT_ITEM "高級品のアイテムを手に入れた。"
#define MES_DIARY_PATRON_CHAOS_WP "(混沌)の武器を手に入れた。"
#define MES_DIARY_PATRON_S_ENEMY "クリーチャーを召喚された。"
#define MES_DIARY_PATRON_TY_CURSE "禍々しい呪いをかけられた。"
#define MES_DIARY_PATRON_HAVOC "カオスの力が渦巻いた。"
#define MES_DIARY_PATRON_GAIN_ABL "能力値が上がった。"
#define MES_DIARY_PATRON_LOST_ABL "能力値が下がった。"
#define MES_DIARY_PATRON_AUGM_ABL "全能力値が上がった。"
#define MES_DIARY_PATRON_RUIN_ABL "全能力値が下がった。"
#define MES_DIARY_PATRON_POLY_WND "傷が変化した。"
#define MES_DIARY_PATRON_HURT_LOT "分解の球が発生した。"
#define MES_DIARY_PATRON_DESTRUCTION "ダンジョンが*破壊*された。"
#define MES_DIARY_PATRON_GENOCIDE "クリーチャーが抹殺された。"
#define MES_DIARY_PATRON_SERVANT "クリーチャーがペットになった。"
#define MES_DIARY_PATRON_DEMONIC_SERVANT "悪魔がペットになった。"
#define MES_DIARY_PATRON_UNDEAD_SERVANT "アンデッドがペットになった。"
#define MES_DIARY_STAND_IN(PLACE) "%sに降り立った。", (PLACE)
#define MES_DIARY_PATRON_HEALING "体力が回復した。"
#define MES_DIARY_CHEAT_DEATH "                            しかし、生き返った。"
#define MES_DIARY_WINNER "見事にD\'angbandの勝利者となった！"
#define MES_DIARY_DISCOVER_ART(ART_NAME) "%sを発見した。\n", (ART_NAME)
#define MES_DIARY_DEFEAT_UNIQUE(UNIQUE_NAME) "%sを倒した。\n", (UNIQUE_NAME)
#define MES_DIARY_FIX_QUEST(QUEST_NAME) "クエスト「%s」を達成した。\n", (QUEST_NAME)
#define MES_DIARY_AWAY_QUEST(QUEST_NAME) " クエスト「%s」から逃げ帰った。\n", (QUEST_NAME)
#define MES_DIARY_FIX_R_QUEST(QUEST_NAME) " ランダムクエスト(%s)を達成した。\n", (QUEST_NAME)
#define MES_DIARY_AWAY_R_QUEST(QUEST_NAME) " ランダムクエスト(%s)から逃げ出した。\n", (QUEST_NAME)
#define MES_DIARY_MAXDEPTH(DUNGEON_NAME, DEPTH) " %sの最深階%d階に到達した。\n", (DUNGEON_NAME), (DEPTH)
#define MES_DIARY_TRUMP_TOWER_SET(NOTE, DUNGEON_NAME, DEPTH) " %s%sの最深階を%d階にセットした。\n", (NOTE), (DUNGEON_NAME), (DEPTH)
#define MES_DIARY_STAIR(TO, NOTE) " %sへ%s。\n", (TO), (NOTE)
#define MES_DIARY_RECALL_DEPTH(DUNGEON_NAME, DEPTH) " 帰還を使って%sの%d階へ下りた。\n", (DUNGEON_NAME), (DEPTH)
#define MES_DIARY_RECALL_SURFACE " 帰還を使って地上へと戻った。\n"
#define MES_DIARY_TO_QUEST(QUEST_NAME) " クエスト「%s」へと突入した。\n", (QUEST_NAME)
#define MES_DIARY_LEV_TELE " レベル・テレポートで脱出した。\n"
#define MES_DIARY_BUY(OBJECT_NAME) " %sを購入した。\n", (OBJECT_NAME)
#define MES_DIARY_SELL(OBJECT_NAME) " %sを売却した。\n", (OBJECT_NAME)
#define MES_DIARY_ARENA_WON(NUM, ENEMY_NAME) " 闘技場の%d回戦(%s)に勝利した。\n", (NUM), (ENEMY_NAME)
#define MES_DIARY_ARENA_LOST(NUM, ENEMY_NAME) " 闘技場の%d回戦で、%sの前に敗れ去った。\n", (NUM), (ENEMY_NAME)
#define MES_DIARY_ARENA_CHAMPION "                 闘技場のすべての敵に勝利し、チャンピオンとなった。\n"
#define MES_DIARY_IDEN(OBJECT_NAME) " %sを識別した。\n", (OBJECT_NAME)
#define MES_DIARY_WIZ_TELE(TO) " %sへとウィザード・テレポートで移動した。\n", (TO)
#define MES_DIARY_PAT_TELE(TO) " %sへとパターンの力で移動した。\n", (TO)
#define MES_DIARY_LEVELUP(LEVEL) " レベルが%dに上がった。\n", (LEVEL)
#else
#define MES_PATRON_BOOM_OUT(PATRON) "The voice of %s booms out:", (PATRON)
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
#define MES_PATRON_WRATH "'Die, mortal!'"
#define MES_PATRON_IGNORE(PATRON) "%s ignores you.", (PATRON)
#define MES_PATRON_DEMON_SERVANT(PATRON) "%s rewards you with a demonic servant!", (PATRON)
#define MES_PATRON_UNDEAD_SERVANT(PATRON) "%s rewards you with an undead servant!", (PATRON)
#define MES_PATRON_SERVANT(PATRON) "%s rewards you with a servant!", (PATRON)
#define MES_PATRON_PRAISE_WINNER "'Thou art donst well, my devotee!'"
#define MES_PATRON_ERROR_REWARD1(PATRON) "The voice of %s stammers:", (PATRON)
#define MES_PATRON_ERROR_REWARD2(TYPE, EFFECT) "'Uh... uh... the answer's %d/%d, what's the question?'", (TYPE), (EFFECT)
#define MES_DIARY_PATRON_DESTROYED(OBJECT) "destroying %s", (OBJECT)
#define MES_DIARY_PATRON_MUTATION "mutation"
#define MES_DIARY_PATRON_GAIN_EXP "experience"
#define MES_DIARY_PATRON_LOST_EXP "losing experience"
#define MES_DIARY_PATRON_GOOD_ITEM "a good item"
#define MES_DIARY_PATRON_GREAT_ITEM "an excellent item"
#define MES_DIARY_PATRON_CHAOS_WP "chaos weapon"
#define MES_DIARY_PATRON_S_ENEMY "summoning hostile creatures"
#define MES_DIARY_PATRON_TY_CURSE "cursing"
#define MES_DIARY_PATRON_HAVOC "calling chaos"
#define MES_DIARY_PATRON_GAIN_ABL "increasing a stat"
#define MES_DIARY_PATRON_LOST_ABL "decreasing a stat"
#define MES_DIARY_PATRON_AUGM_ABL "increasing all stats"
#define MES_DIARY_PATRON_RUIN_ABL "decreasing all stats"
#define MES_DIARY_PATRON_POLY_WND "polymorphing wounds"
#define MES_DIARY_PATRON_HURT_LOT "generating disintegration ball"
#define MES_DIARY_PATRON_DESTRUCTION "*destruct*ing dungeon"
#define MES_DIARY_PATRON_GENOCIDE "genociding creatures"
#define MES_DIARY_PATRON_SERVANT "a servant"
#define MES_DIARY_PATRON_DEMONIC_SERVANT "a demonic servant"
#define MES_DIARY_PATRON_UNDEAD_SERVANT "an undead servant"
#define MES_DIARY_PATRON_HEALING "healing"
#define MES_DIARY_STAND_IN(PLACE) "You are standing in the %s.", (PLACE)
#define MES_DIARY_CHEAT_DEATH "                            but revived."
#define MES_DIARY_WINNER "become *WINNER* of D\'angband finely!"
#define MES_DIARY_DISCOVER_ART(ART_NAME) "discovered %s.\n", (ART_NAME)
#define MES_DIARY_DEFEAT_UNIQUE(UNIQUE_NAME) "defeated %s.\n", (UNIQUE_NAME)
#define MES_DIARY_FIX_QUEST(QUEST_NAME) "completed quest '%s'.\n", (QUEST_NAME)
#define MES_DIARY_AWAY_QUEST(QUEST_NAME) " run away from quest '%s'.\n", (QUEST_NAME)
#define MES_DIARY_FIX_R_QUEST(QUEST_NAME) " completed random quest '%s'\n", (QUEST_NAME)
#define MES_DIARY_AWAY_R_QUEST(QUEST_NAME) " ran away from quest '%s'.\n", (QUEST_NAME)
#define MES_DIARY_MAXDEPTH(DUNGEON_NAME, DEPTH) " reached level %d of %s for the first time.\n", (DUNGEON_NAME), (DEPTH)
#define MES_DIARY_TRUMP_TOWER_SET(NOTE, DUNGEON_NAME, DEPTH) " reset recall level of %s to %d %s.\n", (DUNGEON_NAME), (DEPTH1), (NOTE)
#define MES_DIARY_STAIR(TO, NOTE) " %s %s.\n", (NOTE), (TO)
#define MES_DIARY_RECALL_DEPTH(DUNGEON_NAME, DEPTH) " recalled to dungeon level %d of %s.\n", (DEPTH), (DUNGEON_NAME)
#define MES_DIARY_RECALL_SURFACE " recalled from dungeon to surface.\n"
#define MES_DIARY_TO_QUEST(QUEST_NAME) " entered the quest '%s'.\n", (QUEST_NAME)
#define MES_DIARY_LEV_TELE " Got out using teleport level.\n"
#define MES_DIARY_BUY(OBJECT_NAME) " bought %s.\n", (OBJECT_NAME)
#define MES_DIARY_SELL(OBJECT_NAME) " sold %s.\n", (OBJECT_NAME)
#define MES_DIARY_ARENA_WON(NUM, ENEMY_NAME) " " won the %d%s fight (%s).\n", (NUM),  get_ordinal_number_suffix(n), (ENEMY_NAME)
#define MES_DIARY_ARENA_LOST(NUM, ENEMY_NAME) " beaten by %s in the %d%s fight.\n", (ENEMY_NAME), (NUM),  get_ordinal_number_suffix(n)
#define MES_DIARY_ARENA_CHAMPION "                 won all fight to become a Champion.\n"
#define MES_DIARY_IDEN(OBJECT_NAME) " identified %s.\n", (OBJECT_NAME)
#define MES_DIARY_WIZ_TELE(TO) " wizard-teleport to %s.\n", (TO)
#define MES_DIARY_PAT_TELE(TO) " used Pattern to teleport to %s.\n", (TO)
#define MES_DIARY_LEVELUP(LEVEL) " reached player level %d.\n", (LEVEL)
#endif

#ifdef JP
#define MES_OPTION_CHANGE_CONFIG "設定変更コマンド"
#define MES_OPTION_WARN_CHEAT "詐欺師は決して勝利できない。"
#define MES_OPTION_WARN_CHEAT2 "<<  注意  >>\n詐欺オプションを一度でも設定すると、スコア記録が残らなくなります！\n後に解除してもダメですので、勝利者を目指す方はここのオプションはい\nじらないようにして下さい。"
#define MES_OPTION_UNUSED "(未使用)"
#define MES_OPTION_MAIN_TITLE "[ オプションの設定 ]"
#define MES_OPTION_CMD_HP_WARNING "コマンド: 低ヒットポイント警告"
#define MES_OPTION_CMD_MP_CHECK "コマンド: 低魔力色閾値"
#define MES_OPTION_CMD_MP_PROMPT "低魔力閾値 (0-9) ESCで決定: "
#define MES_OPTION_CMD_HP_CURRENT(PERCENT) "現在の低ヒットポイント警告: %d0%%", (PERCENT)
#define MES_OPTION_CMD_MP_CURRENT(PERCENT) "現在の低魔力色閾値: %d0%%", (PERCENT)
#define MES_OPTION_CMD_HP_PROMPT "低ヒットポイント警告 (0-9) ESCで決定: "
#define MES_OPTION_CMD_AUTOSAVE_PROMPT(INFO) "%s ( リターンで次へ, y/n でセット, F で頻度を入力, ESC で決定 ) ", (INFO)
#define MES_OPTION_CMD_AUTOSAVE_FREQ(TURN) "自動セーブの頻度： %d ターン毎", (TURN)
#define MES_OPTION_CMD_WAIT "コマンド: 基本ウェイト量"
#define MES_OPTION_CMD_VALUE(VALUE) "現在のウェイト: %d (%dミリ秒)", (VALUE), (VALUE)*(VALUE)*(VALUE)
#define MES_OPTION_CMD_WAIT_PROMPT "ウェイト (0-9) ESCで決定: "
#define MES_OPTION_MACRO_PROMPT "マクロ行動: "
#define MES_OPTION_MACRO_DESC "カーソルキーの左右でカーソル位置を移動。BackspaceかDeleteで一文字削除。"
#define MES_OPTION_APPEND_KEYMAP "キー配置を追加しました。"
#define MES_OPTION_FOUND_KEYMAP "キー配置を確認しました。"
#define MES_OPTION_FOUND_NO_KEYMAP "キー配置は定義されていません。"
#define MES_OPTION_REMOVE_KEYMAP "キー配置を削除しました。"
#else
#define MES_OPTION_CHANGE_CONFIG "Pref"
#define MES_OPTION_WARN_CHEAT "Cheaters never win."
#define MES_OPTION_WARN_CHEAT2 ""
#define MES_OPTION_UNUSED "(Unused option)"
#define MES_OPTION_MAIN_TITLE [ Game Options ]"
#define MES_OPTION_CMD_HP_WARNING "Command: Hitpoint Warning"
#define MES_OPTION_CMD_MP_CHECK "Command: Mana Color Threshold"
#define MES_OPTION_CMD_MP_PROMPT "Mana color Threshold (0-9 or ESC to accept): "
#define MES_OPTION_CMD_HP_CURRENT(PERCENT) "Current hitpoint warning: %d0%%", (PERCENT)
#define MES_OPTION_CMD_MP_CURRENT(PERCENT) "Current mana color threshold: %d0%%", (PERCENT)
#define MES_OPTION_CMD_HP_PROMPT "Hitpoint Warning (0-9 or ESC to accept): "
#define MES_OPTION_CMD_AUTOSAVE_PROMPT(INFO) "%s (RET to advance, y/n to set, 'F' for frequency, ESC to accept) ", (INFO)
#define MES_OPTION_CMD_AUTOSAVE_FREQ(TURN) "Timed autosave frequency: every %d turns", (TURN)
#define MES_OPTION_CMD_WAIT "Command: Base Delay Factor"
#define MES_OPTION_CMD_VALUE(VALUE) "現在のウェイト: %d (%dミリ秒)", (VALUE), (VALUE)*(VALUE)*(VALUE)
#define MES_OPTION_CMD_WAIT_PROMPT "Delay Factor (0-9 or ESC to accept): "
#define MES_OPTION_MACRO_PROMPT "Action: "
#define MES_OPTION_MACRO_DESC "Press Left/Right arrow keys to move cursor. Backspace/Delete to delete a char."
#define MES_OPTION_APPEND_KEYMAP "Appended keymaps."
#define MES_OPTION_FOUND_KEYMAP "Found a keymap."
#define MES_OPTION_FOUND_NO_KEYMAP "Found no keymap."
#define MES_OPTION_REMOVE_KEYMAP "Removed a keymap."
#endif

#ifdef JP
#define MES_STATUS_SELECTION "['c'で名前変更, 'f'でファイルへ書出, 'h'でモード変更, ESCで終了]"
#else
#define MES_STATUS_SELECTION "['c' to change name, 'f' to file, 'h' to change mode, or ESC]"
#endif

#ifdef JP
#define MES_DIARY_MENU "記録の設定"
#define MES_DIARY_MENU1 "記録を見る"
#define MES_DIARY_MENU2 "文章を記録する"
#define MES_DIARY_MENU3 "直前に入手又は鑑定したものを記録する"
#define MES_DIARY_MENU4 "記録を消去する"
#define MES_DIARY_MENU5 "プレイ動画を記録する/中止する"
#define MES_DIARY_ASK_ERASE "本当に記録を消去しますか？"
#define MES_DIARY_DELETED "記録を消去しました。"
#define MES_DIARY_DELETE_FAIL(FILE) "%s の消去に失敗しました。", (FILE)
#else
#define MES_DIARY_MENU "Play Record"
#define MES_DIARY_MENU1 "Display your record"
#define MES_DIARY_MENU2 "Add record"
#define MES_DIARY_MENU3 "Record item you last get/identify"
#define MES_DIARY_MENU4 "Delete your record"
#define MES_DIARY_MENU5 "Record playing movie / or stop it"
#define MES_DIARY_ASK_ERASE "Do you really want to delete all your record? "
#define MES_DIARY_DELETED "deleted record."
#define MES_DIARY_DELETE_FAIL(FILE) "failed to delete %s.", (FILE)
#endif

#ifdef JP
#define MES_MUTATION_DONE "体がねじれ始めた..."
#define MES_MUTATION_SOMETHING "何か変わった気がする！"
#else
#define MES_MUTATION_DONE "Your body starts to scramble..."
#define MES_MUTATION_SOMETHING "You feel different!"
#endif

#ifdef JP
#define MES_CAST_BROWSE "読む"
#define MES_CAST_DISABLE(REALM, CAST) "その%sを%sことはできません。", (REALM), (CAST)
#define MES_CAST_WHICH_BOOK "どの呪文書を使いますか? "
#define MES_CAST_NO_BOOK "呪文書がない！"
#define MES_CAST_NONE "呪文を唱えられない！"
#define MES_CAST_NO_NEED_LEARN "呪文を学習する必要はない！"
#define MES_CAST_FAILED(SPELL) "%sをうまく唱えられなかった！", (SPELL)
#define MES_CAST_CHAOS_PENALTY "カオス的な効果を発生した！"
#define MES_CAST_NECRO_PENALTY1 "ネクロノミコンを読んで正気を失った！"
#define MES_CAST_NECRO_PENALTY2 "痛い！"
#define MES_CAST_MUSIC_PENALTY "いやな音が響いた"
#define MES_CAST_FAINT "精神を集中しすぎて気を失ってしまった！"
#define MES_CAST_DAMAGE_HEALTH "体を悪くしてしまった！"
#define MES_CAST_WHICH_KNOW(SKILL, FROM, TO) "(%^s %c-%c, '*'で一覧, ESC) どの%sについて知りますか？", (SKILL), (FROM), (TO), (SKILL)
#define MES_CAST_WHICH_USE(SKILL, FROM, TO) "(%^s %c-%c, '*'で一覧, ESC) どの%sを使いますか？", (SKILL), (FROM), (TO), (SKILL)
#define MES_CAST_ASK(SPELL) "%sの魔法を唱えますか？", (SPELL)

#else
#define MES_CAST_BROWSE "browse"
#define MES_CAST_DISABLE(REALM, CAST) "You may not %s that %s.", (CAST), (REALM)
#define MES_CAST_WHICH_BOOK "Use which book? "
#define MES_CAST_NO_BOOK "You have no spell books!"
#define MES_CAST_NONE "You cannot cast spells!"
#define MES_CAST_NO_NEED_LEARN "You don't have to learn spells!"
#define MES_CAST_FAILED(SPELL) "You failed to get the %s off!", (SPELL)
#define MES_CAST_CHAOS_PENALTY "You produce a chaotic effect!"
#define MES_CAST_NECRO_PENALTY1 "Your sanity is shaken by reading the Necronomicon!"
#define MES_CAST_NECRO_PENALTY2 "It hurts!"
#define MES_CAST_MUSIC_PENALTY "An infernal sound echoed."
#define MES_CAST_FAINT "You faint from the effort!"
#define MES_CAST_DAMAGE_HEALTH "You have damaged your health!"
#define MES_CAST_WHICH_KNOW(SKILL, FROM, TO) "(%^ss %c-%c, *=List, ESC=exit) Browse which %s? ", (SKILL), (FROM), (TO), (SKILL)
#define MES_CAST_WHICH_USE(SKILL, FROM, TO) "(%^ss %c-%c, *=List, ESC=exit) Use which %s? ", (SKILL), (FROM), (TO), (SKILL)
#define MES_CAST_ASK(SPELL) "Use %s? ", (SPELL)
#endif

#ifdef JP 
#define MES_DEBUG_FORBID "デバッグコマンドは許可されていません。 "
#define MES_DEBUG_CHECK1 "デバッグ・コマンドはデバッグと実験のためのコマンドです。"
#define MES_DEBUG_CHECK2 "デバッグ・コマンドを使うとスコアは記録されません。"
#define MES_DEBUG_ASK "本当にデバッグ・コマンドを使いますか? "
#define MES_WIZARD_FORBID "デバッグコマンドは許可されていません。 "
#define MES_WIZARD_CHECK1 "ウィザードモードはデバッグと実験のためのモードです。 "
#define MES_WIZARD_CHECK2 "一度ウィザードモードに入るとスコアは記録されません。"
#define MES_WIZARD_ASK "本当にウィザードモードに入りたいのですか? "
#else
#define MES_DEBUG_FORBID "Use of debug command is not permitted."
#define MES_DEBUG_CHECK1 "The debug commands are for debugging and experimenting."
#define MES_DEBUG_CHECK2 "The game will not be scored if you use debug commands."
#define MES_DEBUG_ASK "Are you sure you want to use debug commands? "
#define MES_WIZARD_FORBID "Wizard mode is not permitted."
#define MES_WIZARD_CHECK1 "Wizard mode is for debugging and experimenting."
#define MES_WIZARD_CHECK2 "The game will not be scored if you enter wizard mode."
#define MES_WIZARD_ASK "Are you sure you want to enter wizard mode? "
#endif

#ifdef JP 
#define MES_STEED_RIDE_ON(STEED) "%sに乗った。", (STEED)->name
#define MES_STEED_TAMED(STEED) "%sを手なずけた。", (STEED)->name
#define MES_STEED_TAME_FAILED(STEED) "%sに振り落とされた！", (STEED)->name
#define MES_STEED_GOT_OFF(STEED) "%sから降りた。", (STEED)
#define MES_PET_DISMISS_ASK(PET, REMAIN) "%sを放しますか？ [Yes/No/Unnamed (%d体)]", (PET), (REMAIN)
#define MES_PET_RELEASE(PET) "%s を放した。", (PET)
#define MES_PET_DISMISS(NUM) "%d 体のペットを放しました。", (NUM)
#define MES_PET_NOTHING "ペットがいない！"
#define MES_PET_NO_PET "そのクリーチャーはペットではない。"
#define MES_PET_CANNOT_CHANGE_NAME "そのクリーチャーの名前は変えられない。"
#define MES_PET_CHANGE_NAME(NAME) "%sに名前をつける。", (NAME)
#define MES_PER_UNNAMED_DESC "'U'nnamed は、乗馬以外の名前のないペットだけを全て解放します。"
#define MES_STEED_BUMP_WALL(STEED) "%sから振り落とされそうになって、壁にぶつかった。", (STEED)
#define MES_STEED_GOOD_RANDING(STEED) "%sから落ちたが、空中でうまく体勢を立て直して着地した。", (STEED)
#define MES_STEED_WAKE_UP(STEED) "%sを起こした。", (STEED)
#define MES_PET_OVER_UPKEEP "こんなに多くのペットを制御できない！"
#define MES_PET_DISP_COST(UPKEEP_FACTOR) "維持ＭＰは %d%%", (UPKEEP_FACTOR)
#else
#define MES_STEED_GOT_OFF(STEED) "You have got off %s. ", (STEED)
#define MES_STEED_TAMED(STEED) "You tame %s.", (STEED)->name;
#define MES_STEED_TAME_FAILED(STEED) "You have thrown off by %s.", (STEED)->name
#define MES_PET_DISMISS_ASK(PET, REMAIN) "Dismiss %s? [Yes/No/Unnamed (%d remain)]", (PET), (REMAIN)
#define MES_PET_RELEASE(PET) "Dismissed %s.", (PET)
#define MES_PET_DISMISS(NUM) "You have dismissed %d pet%s.", (NUM), ((NUM) == 1 ? "" : "s")
#define MES_PET_NOTHING "You have no pets."
#define MES_PET_NO_PET "This creature is not a pet."
#define MES_PET_CANNOT_CHANGE_NAME "You cannot change name of this creature."
#define MES_PET_CHANGE_NAME(NAME) "Name %s.", (NAME)
#define MES_PER_UNNAMED_DESC "'U'nnamed means all your pets except named pets and your mount."
#define MES_STEED_BUMP_WALL(STEED) "You have nearly fallen from %s, but bumped into wall.", (STEED)
#define MES_STEED_GOOD_RANDING(STEED) "You are thrown from %s, but make a good landing.", (STEED)
#define MES_STEED_WAKE_UP(STEED) "You have waked %s up.", (STEED)
#define MES_PET_OVER_UPKEEP "Too many pets to control at once!"
#define MES_PET_DISP_COST(UPKEEP_FACTOR) "Upkeep: %d%% mana.", (UPKEEP_FACTOR)
#endif

#ifdef JP
#define MES_PETCOM_DISMISS_PET "ペットを放す"
#define MES_PETCOM_TARGETING(CURRENT) "ペットのターゲットを指定 (現在：%s)", (CURRENT)
#define MES_PETCOM_STAY_CLOSE "近くにいろ"
#define MES_PETCOM_FOLLOW_ME "ついて来い"
#define MES_PETCOM_SEEK_AND_DESTROY "敵を見つけて倒せ"
#define MES_PETCOM_GIVE_ME_SPACE "少し離れていろ"
#define MES_PETCOM_STAY_AWAY "離れていろ"
#define MES_PETCOM_OPEN_DOORS_ON "ドアを開ける (現在:ON)"
#define MES_PETCOM_OPEN_DOORS_OFF "ドアを開ける (現在:OFF)"
#define MES_PETCOM_OPEN_PICKUP_ITEM_ON "アイテムを拾う (現在:ON)"
#define MES_PETCOM_OPEN_PICKUP_ITEM_OFF "アイテムを拾う (現在:OFF)"
#define MES_PETCOM_ALLOW_TELEPORT_ON "テレポート系魔法を使う (現在:ON)"
#define MES_PETCOM_ALLOW_TELEPORT_OFF "テレポート系魔法を使う (現在:OFF)"
#define MES_PETCOM_ATTACK_SPELL_ON "攻撃魔法を使う (現在:ON)"
#define MES_PETCOM_ATTACK_SPELL_OFF "攻撃魔法を使う (現在:OFF)"
#define MES_PETCOM_SUMMON_SPELL_ON "召喚魔法を使う (現在:ON)"
#define MES_PETCOM_SUMMON_SPELL_OFF "召喚魔法を使う (現在:OFF)"
#define MES_PETCOM_INVOLVE_ON "プレイヤーを巻き込む範囲魔法を使う (現在:ON)"
#define MES_PETCOM_INVOLVE_OFF "プレイヤーを巻き込む範囲魔法を使う (現在:OFF)"
#define MES_PETCOM_GET_OFF "ペットから降りる"
#define MES_PETCOM_RIDE "ペットに乗る"
#define MES_PETCOM_GIVE_PET_NAME "ペットに名前をつける"
#else
#define MES_PETCOM_DISMISS_PET "dismiss pets"
#define MES_PETCOM_TARGETING(CURRENT) "specify a target of pet (now:%s)", (CURRENT)
#define MES_PETCOM_STAY_CLOSE "stay close"
#define MES_PETCOM_FOLLOW_ME "follow me"
#define MES_PETCOM_SEEK_AND_DESTROY "seek and destroy"
#define MES_PETCOM_GIVE_ME_SPACE "give me space"
#define MES_PETCOM_STAY_AWAY "stay away"
#define MES_PETCOM_OPEN_DOORS_ON "pets open doors (now On)"
#define MES_PETCOM_OPEN_DOORS_OFF "pets open doors (now Off)"
#define MES_PETCOM_OPEN_PICKUP_ITEM_ON "pets pick up items (now On)"
#define MES_PETCOM_OPEN_PICKUP_ITEM_OFF "pets pick up items (now Off)"
#define MES_PETCOM_ALLOW_TELEPORT_ON "allow teleport (now On)"
#define MES_PETCOM_ALLOW_TELEPORT_OFF "allow teleport (now Off)"
#define MES_PETCOM_ATTACK_SPELL_ON "allow cast attack spell (now On)"
#define MES_PETCOM_ATTACK_SPELL_OFF "allow cast attack spell (now Off)"
#define MES_PETCOM_SUMMON_SPELL_ON "allow cast summon spell (now On)"
#define MES_PETCOM_SUMMON_SPELL_OFF "allow cast summon spell (now Off)"
#define MES_PETCOM_INVOLVE_ON "allow involve player in area spell (now On)"
#define MES_PETCOM_INVOLVE_OFF "allow involve player in area spell (now Off)"
#define MES_PETCOM_GET_OFF "get off a pet"
#define MES_PETCOM_RIDE "ride a pet"
#define MES_PETCOM_GIVE_PET_NAME "name pets"
#endif

#ifdef JP
#define MES_TRAIT_DEFAULT_ACTIVATE(CASTER, TITLE) "%sは%sを発動した。", (CASTER), (TITLE)
#define MES_TRAIT_CRITICAL_STUN "割れるような頭痛がする。"
#define MES_TRAIT_CUT_PENALTY(CREATURE, PLAYER) "%sにひどい傷跡が残った。", (CREATURE)
#define MES_TRAIT_RUINATION "身も心も弱ってきて、精気が抜けていくようだ。"
#define MES_TRAIT_KNOWLEDGE "自分自身のことが少しは分かった気がする..."
#define MES_TRAIT_OKURE_NIISAN "「オクレ兄さん！」"
#define MES_TRAIT_SELF_DETONATION "体の中で激しい爆発が起きた！"
#define MES_TRAIT_CLOUDY_MIND "精神にもやがかかった！"
#define MES_TRAIT_EXPAND_MIND "精神が広がった！"
#define MES_TRAIT_WASTING "自分が衰弱していくのが分かる！"
#define MES_TRAIT_ATT_DRAGON "ドラゴンを引き寄せた！"
#define MES_TRAIT_ATT_ANIMAL "動物を引き寄せた！"
#define MES_TRAIT_NAUSEA "胃が痙攣し、食事を失った！"
#define MES_TRAIT_ADD_CURSED(OBJECT) "悪意に満ちた黒いオーラが%sをとりまいた...", (OBJECT)->name
#define MES_TRAIT_BERS_RAGE1 "ウガァァア！"
#define MES_TRAIT_BERS_RAGE2 "激怒の発作に襲われた！"
#define MES_TRAIT_COWARDICE "とても暗い... とても恐い！"
#define MES_TRAIT_CURSED_TELEPORT "あなたの位置は突然ひじょうに不確定になった..."
#define MES_TRAIT_BANISH_NO_TARGET "邪悪な存在を感じとれません！"
#define MES_TRAIT_BANISH_DONE "邪悪なクリーチャーは硫黄臭い煙とともに消え去った！"
#define MES_TRAIT_BANISH_UNAFFECTED "祈りは効果がなかった！"
#define MES_TRAIT_FLATULENT "ブゥーーッ！おっと。"
#define MES_TRAIT_PROD_MANA "魔法のエネルギーが突然あなたの中に流れ込んできた！エネルギーを解放しなければならない！"
#define MES_TRAIT_VAMPIRISM_NO_TARGET "何もない場所に噛みついた！"
#define MES_TRAIT_VAMPIRISM_DONE "あなたはニヤリとして牙をむいた..."
#define MES_TRAIT_VAMPIRISM_FAILED "げぇ。ひどい味だ。"
#define MES_TRAIT_VAMPIRISM_NO_HUNGER "あなたは空腹ではありません。"
#define MES_TRAIT_EAT_ROCK_CANNOT "この地形は食べられない。"
#define MES_TRAIT_EAT_ROCK_PERMANENT(FEATURE_NAME) "いてっ！この%sはあなたの歯より硬い！", (FEATURE_NAME)
#define MES_TRAIT_ACROBAT(CREATURE) "%^sは突然視界から消えた!", (CREATURE)->name
#define MES_TRAIT_ACROBAT_EFFECT(CREATURE) "%^sがあなたを掴んで空中から投げ落とした。", (CREATURE)->name
#define MES_TRAIT_ACROBAT_FLOAT_GENTLY(CREATURE) "%^sは静かに着地した。", (CREATURE)->name
#define MES_TRAIT_ACROBAT_CRUSH(CREATURE) "%^sは地面に叩きつけられた。", (CREATURE)->name
#define MES_TRAIT_INROU_SUKE "『助さん』が現れた。"
#define MES_TRAIT_INROU_KAKU "『格さん』が現れた。"
#define MES_TRAIT_BR_SOUN_JAIAN "「ボォエ～～～～～～」"
#define MES_TRAIT_BR_SHAR_BOTEI "「ボ帝ビルカッター！！！」"
#define MES_TRAIT_BO_FIRE_RORENTO_BLIND(CASTER) "%sが何かを投げた。", (CASTER)->name
#define MES_TRAIT_BO_FIRE_RORENTO(CASTER) "%sは手榴弾を投げた。", (CASTER)->name
#define MES_TRAIT_BLINK_DONE(CASTER) "%^sが瞬時に消えた。", (CASTER)->name
#define MES_TRAIT_TELEPORT_DONE(CASTER) "%^sがテレポートした。", (CASTER)->name
#define MES_TRAIT_TELEPORT_BLOCK(CASTER) "魔法のバリアが%^sのテレポートを邪魔した。", (CASTER)->name
#define MES_TRAIT_SPLIT_TWO_MEN(CASTER) "%^sが分裂した！", (CASTER)->name
#define MES_TRAIT_COMBINE "『バーノール』と『ルパート』が合体した！"
#define MES_TRAIT_TELEPORT_BACK_DONE(TARGET) "%sが引き戻された。", (TARGET)->name
#define MES_TRAIT_WATER_FLOW "地面から水が吹き出した！"
#define MES_TRAIT_S_CYBER_BLIND "重厚な足音が近くで聞こえる。"
#define MES_TRAIT_RUSH1 "あーたたたたたたたたたたたたたたたたたたたたたた！！！"
#define MES_TRAIT_RUSH2 "オラオラオラオラオラオラオラオラオラオラオラオラ！！！"
#define MES_TRAIT_RUSH3 "無駄無駄無駄無駄無駄無駄無駄無駄無駄無駄無駄無駄！！！"
#define MES_TRAIT_CONCENTRATION "精神を集中して気合いを溜めた。"
#define MES_TRAIT_SWORD_DANCING_MISS(CASTER) "$^sの攻撃が空をきった。", (CASTER)->name
#define MES_TRAIT_EAT_ROCK_HATE_TREE "木の味は好きじゃない！"
#define MES_TRAIT_EAT_ROCK_HATE_GLASS "ガラスの味は好きじゃない！"
#define MES_TRAIT_EAT_ROCK_DONE(FEATURE) "この%sはとてもおいしい！", (FEATURE)
#define MES_TRAIT_RECALL_UP "上に引っ張りあげられる感じがする！"
#define MES_TRAIT_RECALL_DOWN "下に引きずり降ろされる感じがする！"
#define MES_TRAIT_ALTER_REAL_DONE "世界が変わった！"
#define MES_TRAIT_ALTER_REAL_FAILED "世界が少しの間変化したようだ。"
#else
#define MES_TRAIT_DEFAULT_ACTIVATE(CASTER, TITLE) "%s invoked '%s'", (CASTER), (TITLE)
#define MES_TRAIT_CRITICAL_STUN "A vicious blow hits your head."
#define MES_TRAIT_CUT_PENALTY(CREATURE, PLAYER) "%s %s been horribly scarred.", (CREATURE), (PLAYER) ? "have" : "has"
#define MES_TRAIT_RUINATION "Your nerves and muscles feel weak and lifeless!"
#define MES_TRAIT_KNOWLEDGE "You begin to know yourself a little better..."
#define MES_TRAIT_OKURE_NIISAN "Brother OKURE!"
#define MES_TRAIT_SELF_DETONATION "Massive explosions rupture your body!"
#define MES_TRAIT_CLOUDY_MIND "Your mind feels cloudy!"
#define MES_TRAIT_EXPAND_MIND "Your mind expands!"
#define MES_TRAIT_WASTING "You can feel yourself wasting away!"
#define MES_TRAIT_ATT_DRAGON "You have attracted a dragon!"
#define MES_TRAIT_ATT_ANIMAL "You have attracted an animal!"
#define MES_TRAIT_NAUSEA "Your stomach roils, and you lose your lunch!"
#define MES_TRAIT_ADD_CURSED(OBJECT) "There is a malignant black aura surrounding your %s...", (OBJECT)->name
#define MES_TRAIT_BERS_RAGE1 "RAAAAGHH!"
#define MES_TRAIT_BERS_RAGE2 "You feel a fit of rage coming over you!"
#define MES_TRAIT_COWARDICE "It's so dark... so scary!"
#define MES_TRAIT_CURSED_TELEPORT "Your position suddenly seems very uncertain..."
#define MES_TRAIT_BANISH_NO_TARGET "You sense no evil there!"
#define MES_TRAIT_BANISH_DONE "The evil creature vanishes in a puff of sulfurous smoke!"
#define MES_TRAIT_BANISH_UNAFFECTED "The invocation is ineffectual."
#define MES_TRAIT_FLATULENT "BRRAAAP! Oops."
#define MES_TRAIT_PROD_MANA "Magical energy flows through you! You must release it!"
#define MES_TRAIT_VAMPIRISM_NO_TARGET "You bite into thin air!"
#define MES_TRAIT_VAMPIRISM_DONE "You grin and bare your fangs..."
#define MES_TRAIT_VAMPIRISM_FAILED "Yechh. That tastes foul."
#define MES_TRAIT_VAMPIRISM_NO_HUNGER "You were not hungry."
#define MES_TRAIT_EAT_ROCK_CANNOT "You cannot eat this feature."
#define MES_TRAIT_EAT_ROCK_PERMANENT(FEATURE_NAME) "Ouch!  This %s is harder than your teeth!", (FEATURE_NAME)
#define MES_TRAIT_ACROBAT(CREATURE) "%^s suddenly go out of your sight!", (CREATURE)->name
#define MES_TRAIT_ACROBAT_EFFECT(CREATURE) "%^s holds you, and drops from the sky.", (CREATURE)->name
#define MES_TRAIT_ACROBAT_FLOAT_GENTLY(CREATURE) "You floated gently down to the ground.", (CREATURE)->name
#define MES_TRAIT_ACROBAT_CRUSH(CREATURE) "You crashed into the ground.", (CREATURE)->name
#define MES_TRAIT_INROU_SUKE "Suke-san apperars."
#define MES_TRAIT_INROU_KAKU "Kaku-san appears."
#define MES_TRAIT_BR_SOUN_JAIAN "'Booooeeeeee'"
#define MES_TRAIT_BR_SHAR_BOTEI "'Boty-Build cutter!!!'"
#define MES_TRAIT_BO_FIRE_RORENTO_BLIND(CASTER) "%^s throws something.", (CASTER)->name
#define MES_TRAIT_BO_FIRE_RORENTO(CASTER) "%^s throws a hand grenade.", (CASTER)->name
#define MES_TRAIT_BLINK_DONE(CASTER) "%^s blinks", (CASTER)->name
#define MES_TRAIT_TELEPORT_DONE(CASTER) "%^s teleports away.", (CASTER)->name
#define MES_TRAIT_TELEPORT_BLOCK(CASTER) "Magic barrier obstructs teleporting of %^s.", (CASTER)->name
#define MES_TRAIT_SPLIT_TWO_MEN(CASTER) "%^s splits in two person!", (CASTER)->name
#define MES_TRAIT_COMBINE "Banor and Rupart combine into one!"
#define MES_TRAIT_TELEPORT_BACK_DONE(TARGET) "You command %s to return.", (TARGET)->name
#define MES_TRAIT_WATER_FLOW "Water blew off from the ground!"
#define MES_TRAIT_S_CYBER_BLIND "You hear heavy steps nearby."
#define MES_TRAIT_RUSH1 "Ahhhtatatatatatatatatatatatatatataatatatatattaaaaa!!!!"
#define MES_TRAIT_RUSH2 "Oraoraoraoraoraoraoraoraoraoraoraoraoraoraoraoraora!!!!"
#define MES_TRAIT_RUSH3 "Mudamudamudamudamudamudamudamudamudamudamudamudamudamudamudarrrr!!!!"
#define MES_TRAIT_CONCENTRATION "You concentrate to charge your power."
#define MES_TRAIT_SWORD_DANCING_MISS(CASTER) "$^s attack the empty air.", (CASTER)->name
#define MES_TRAIT_EAT_ROCK_HATE_TREE "You don't like the woody taste!"
#define MES_TRAIT_EAT_ROCK_HATE_GLASS "You don't like the glassy taste!"
#define MES_TRAIT_EAT_ROCK_DONE(FEATURE) "This %s is very filling!", (FEATURE)
#define MES_TRAIT_RECALL_UP "You feel yourself yanked upwards!"
#define MES_TRAIT_RECALL_DOWN "You feel yourself yanked downwards!"
#define MES_TRAIT_ALTER_REAL_DONE "The world changes!"
#define MES_TRAIT_ALTER_REAL_FAILED "The world seems to change for a moment!"
#endif

#ifdef JP
#define MES_POSTURE_NO_POSTURE "もともと構えていない。"
#define MES_POSTURE_S_STEALTH_LITE "影の覆いが薄れた気がする。"
#define MES_POSTURE_S_STEALTH_DARK "影の覆いが濃くなった！"
#define MES_POSTURE_REASSUME "構え直した。"
#define MES_POSTURE_ASSUMED(POSTURE) "%sの構えをとった。", (POSTURE)
#else
#define MES_POSTURE_NO_POSTURE "You are not assuming a posture."
#define MES_POSTURE_S_STEALTH_LITE "Your mantle of shadow become thin."
#define MES_POSTURE_S_STEALTH_DARK "Your mantle of shadow restored its original darkness."
#define MES_POSTURE_REASSUME "You reassume a posture."
#define MES_POSTURE_ASSUMED(POSTURE) "You assume a posture of %s form.", (POSTURE)
#endif

#ifdef JP
#define MES_SNIPE_CONCENTRATE(LEVEL) "集中した。(集中度 %d)", (LEVEL)
#define MES_SNIPE_RESET_CONS "集中力が途切れた。"
#define MES_SNIPE_STRAIN "射撃の反動が体を襲った。"
#else
#define MES_SNIPE_CONCENTRATE(LEVEL) "You concentrate deeply. (level %d)", (LEVEL)
#define MES_SNIPE_RESET_CONS "Stop concentrating."
#define MES_SNIPE_STRAIN "A reactionary of shooting attacked you. "
#endif

#ifdef JP
#define MES_ENCHANT_WHICH_ITEM "どのアイテムを強化しますか? "
#define MES_ENCHANT_NO_ITEM "強化できるアイテムがない。"
#define MES_ENCHANT_WHICH_WEAPON "どの武器を強化しますか? "
#define MES_ENCHANT_NO_WEAPON "強化できる武器がない。"
#define MES_ENCHANT_FAILED "属性付加に失敗した。"
#define MES_ENCHANT_SUCCESS(ITEM, POS, NUM) "%sは明るく輝いた！", (ITEM)
#define MES_ENCHANT_BECOME_ARTIFACT(ITEM, POS, NUM) "%sは眩い光を発した！", (ITEM)
#define MES_ENCHANT_RANGE(LIMIT) "現在のあなたの技量だと、+%d まで改良できます。", (LIMIT)
#define MES_ENCHANT_SHORTAGE_GOLD(ITEM) "%sを改良するだけのゴールドがありません！", (ITEM)
#define MES_ENCHANT_COST(COST) " 改良の料金は一個につき＄%d です。", (COST)
#define MES_ENCHANT_DONE(COST, OBJECT) "＄%dで%sに改良しました。", (COST), (OBJECT)->name
#define MES_RESIZE_RANGE(LIMIT) "現在のあなたの技量だと、-%dから+%d まで修正できます。", (LIMIT), (LIMIT)
#define MES_RESIZE_DEPEND "修正の料金はアイテムの価値に依存します。"
#define MES_RESIZE_COST(OBJECT, COST) "%sの改良には $%d かかります！", (OBJECT)->name, (COST)
#define MES_RESIZE_COST_ASK(OBJECT, COST) "%sの改良には $%d かかります、よろしいですか？", (OBJECT)->name, (COST)
#define MES_RESIZE_NO_NEED "改良の必要はありません。"
#define MES_RESIZE_DONE(OBJECT) "%sの大きさを調整した。", (OBJECT)->name
#else
#define MES_ENCHANT_WHICH_ITEM "Enchant which item? "
#define MES_ENCHANT_NO_ITEM "You have nothing to enchant."
#define MES_ENCHANT_WHICH_WEAPON "Enchant which weapon? "
#define MES_ENCHANT_NO_WEAPON "You have nothing to enchant."
#define MES_ENCHANT_FAILED "The Branding failed."
#define MES_ENCHANT_SUCCESS(ITEM, POS, NUM) "%s %s glow%s brightly!", (((POS) >= 0) ? "Your" : "The"), (ITEM), (((NUM) > 1) ? "" : "s")
#define MES_ENCHANT_BECOME_ARTIFACT(ITEM, POS, NUM)) "%s %s radiate%s a blinding light!", (((POS) >= 0) ? "Your" : "The"), (ITEM), (((NUM) > 1) ? "" : "s")
#define MES_ENCHANT_RANGE(LIMIT) "  Based on your skill, we can improve up to +%d.", (LIMIT)
#define MES_ENCHANT_SHORTAGE_GOLD(ITEM) "You do not have the gold to improve %s!", (ITEM)
#define MES_ENCHANT_COST(COST) "  The price for the service is %d gold per item.", (COST)
#define MES_ENCHANT_DONE(COST, OBJECT) "Improved into %s for %d gold.", (OBJECT)->name, (COST)
#define MES_RESIZE_RANGE(LIMIT) "  Based on your skill, we can resize from -%d to +%d.", (LIMIT), (LIMIT)
#define MES_RESIZE_DEPEND "  The price for the service depend on value of the item."
#define MES_RESIZE_COST(OBJECT, COST) "To improve %s cost $%d!", (OBJECT)->name, (COST)
#define MES_RESIZE_COST_ASK(OBJECT, COST) "To improve %s cost $%d, all right?", (OBJECT)->name, (COST)
#define MES_RESIZE_NO_NEED "No improvement is required."
#define MES_RESIZE_DONE(OBJECT) "%s was made over.", (OBJECT)->name
#endif

#ifdef JP
#define MES_POKER_ROYAL_FLUSH "ロイヤルストレートフラッシュ"
#define MES_POKER_STRAIGHT_FLUSH "ストレートフラッシュ"
#define MES_POKER_STRAIGHT "ストレート"
#define MES_POKER_FLUSH "フラッシュ"
#define MES_POKER_ONE_PAIR "ワンペア"
#define MES_POKER_TWO_PAIR "ツーペア"
#define MES_POKER_THREE_OF_A_KIND "スリーカード"
#define MES_POKER_FULL_HOUSE "フルハウス"
#define MES_POKER_FOUR_OF_A_KIND "フォーカード"
#define MES_POKER_FIVE_ACE "ファイブエース"
#define MES_POKER_FIVE_OF_A_KIND "ファイブカード"
#else
#define MES_POKER_ROYAL_FLUSH "Royal Flush"
#define MES_POKER_STRAIGHT_FLUSH "Straight Flush"
#define MES_POKER_STRAIGHT "Straight"
#define MES_POKER_FLUSH "Flush"
#define MES_POKER_ONE_PAIR "One pair"
#define MES_POKER_TWO_PAIR "Two pair"
#define MES_POKER_THREE_OF_A_KIND "Three of a kind"
#define MES_POKER_FULL_HOUSE "Full house"
#define MES_POKER_FOUR_OF_A_KIND "Four of a kind"
#define MES_POKER_FIVE_ACE "Five ace"
#define MES_POKER_FIVE_OF_A_KIND "Five of a kind"
#endif

#ifdef JP
#define MES_CURSE_PREVENT_TAKE_OFF "ふーむ、どうやら呪われているようだ。"
#define MES_CURSE_PREVENT_TAKE_OFF2 "なんてこった！あなたは神域の力に呪われている！"
#define MES_CURSE_PREVENT_TAKE_OFF3 "装備を外せなかった。"
#define MES_CURSE_PREVENT_TAKE_OFF4(OBJECT) "%sは呪われているようだ。", (OBJECT)->name
#define MES_CURSE_RESIST_FORCE "呪われた装備を力づくで剥がした！"
#define MES_CURSE_RESIST_DIVINE "あなたの神域の力は呪いを凌駕している。あなたは平然と呪いの装備を外した。"
#else
#define MES_CURSE_PREVENT_TAKE_OFF "Hmmm, it seems to be cursed."
#define MES_CURSE_PREVENT_TAKE_OFF2 "What the hell! You are cursed by divine power!"
#define MES_CURSE_PREVENT_TAKE_OFF3 "You couldn't remove the equipment."
#define MES_CURSE_PREVENT_TAKE_OFF4(OBJECT) "The %s appears to be cursed.", (OBJECT)->name
#define MES_CURSE_RESIST_FORCE "You teared a cursed equipment off by sheer strength!"
#define MES_CURSE_RESIST_DIVINE "Your divine power is exceeding curse. You teared a cursed equipment off calmly."
#endif

#ifdef JP
#define MES_FILE_SAVED(FILENAME) "%sに書き込みました。", (FILENAME)
#define MES_FILE_SAVED_FAIL(FILENAME) "%sへの書き込みに失敗しました。", (FILENAME)
#define MES_FILE_LOADED(FILENAME) "%sを読み込みました。", (FILENAME)
#define MES_FILE_LOADED_FAIL(FILENAME) "%sの読み込みに失敗しました。", (FILENAME)
#else
#define MES_FILE_SAVED(FILENAME) "Saved '%s'.", (FILENAME)
#define MES_FILE_SAVED_FAIL(FILENAME) "Failed to save '%s'.", (FILENAME)
#define MES_FILE_LOADED(FILENAME) "Loaded '%s'.", (FILENAME)
#define MES_FILE_LOADED_FAIL(FILENAME) "Failed to load '%s'.", (FILENAME)
#endif

#ifdef JP
#define MES_INSCRIPTION_VIEW(FEATURE) "%sにメッセージが刻まれている:", (FEATURE)
#else
#define MES_INSCRIPTION_VIEW(FEATURE) "You find the following inscription on %s.", (FEATURE)
#endif

#ifdef JP
#define MES_RACIAL_RISK_ASK "本当に今の衰弱した状態でこの能力を使いますか？"
#define MES_RACIAL_FAILED "充分に集中できなかった。"
#else
#define MES_RACIAL_RISK_ASK "Really use the power in your weakened state? "
#define MES_RACIAL_FAILED "You've failed to concentrate hard enough."
#endif

#ifdef JP
#define MES_MACRO_ADDED "マクロを追加しました。"
#define MES_MACRO_NOT_FOUND "そのキーにはマクロは定義されていません。"
#define MES_MACRO_FOUND "マクロを確認しました。"
#define MES_MACRO_DELETE "マクロを削除しました。"
#else
#define MES_MACRO_ADDED "Appended macros."
#define MES_MACRO_NOT_FOUND "Found no macro."
#define MES_MACRO_FOUND "Found a macro"
#define MES_MACRO_DELETE "Removed a macro."
#endif

#ifdef JP
#define MES_INSCRIPTION_DONE "自分の真下にメッセージを刻む:"
#define MES_INSCRIPTION_VANISHED "メッセージは消え去った。"
#else
#define MES_INSCRIPTION_DONE "Inscribe messages on your floor."
#define MES_INSCRIPTION_VANISHED "Messages vanished."
#endif

#ifdef JP
#define MES_LITE_FAINT "明かりが微かになってきている。"
#define MES_LITE_OUT "明かりが消えてしまった！"
#define MES_LITE_NONE "光源を装備していない。"
#define MES_LITE_NO_REFILL "この光源は寿命を延ばせない。"
#define MES_LITE_FUEL_LAMP "ランプに油を注いだ。"
#define MES_LITE_FUEL_GONE "ランプが消えてしまった！"
#define MES_LITE_FUEL_DARKEN "しかしランプは全く光らない。"
#define MES_LITE_FUEL_FULL "ランプの油は一杯だ。"
#define MES_LITE_TORCH_COMBINE "松明を結合した。"
#define MES_LITE_TORCH_GONE "松明が消えてしまった！"
#define MES_LITE_TORCH_DARKEN "しかし松明は全く光らない"
#define MES_LITE_TORCH_FULL "松明の寿命は十分だ。"
#define MES_LITE_TORCH_GLOW "松明はいっそう明るく輝いた。"
#define MES_LITE_PHLOGISTON_NONE "燃素を消費するアイテムを装備していません。"
#define MES_LITE_PHLOGISTON_NO_MORE "このアイテムにはこれ以上燃素を補充できません。"
#define MES_LITE_PHLOGISTON_REFUEL "照明用アイテムに燃素を補充した。"
#define MES_LITE_PHLOGISTON_FULL "照明用アイテムは満タンになった。"
#else
#define MES_LITE_FAINT "Your light is growing faint."
#define MES_LITE_OUT "Your light has gone out!"
#define MES_LITE_NONE "You are not wielding a light."
#define MES_LITE_NO_REFILL "Your light cannot be refilled."
#define MES_LITE_FUEL_LAMP "You fuel your lamp."
#define MES_LITE_FUEL_GONE "Your lamp has gone out!"
#define MES_LITE_FUEL_DARKEN "Curiously, your lamp doesn't light."
#define MES_LITE_FUEL_FULL "Your lamp is full."
#define MES_LITE_TORCH_COMBINE "You combine the torches."
#define MES_LITE_TORCH_GONE "Your torch has gone out!"
#define MES_LITE_TORCH_DARKEN "Curiously, your torche don't light."
#define MES_LITE_TORCH_FULL "Your torch is fully fueled."
#define MES_LITE_TORCH_GLOW "Your torch glows more brightly."
#define MES_LITE_PHLOGISTON_NONE "You are not wielding anything which uses phlogiston
#define MES_LITE_PHLOGISTON_NO_MORE "No more phlogiston can be put in this item."
#define MES_LITE_PHLOGISTON_REFUEL "You add phlogiston to your light item."
#define MES_LITE_PHLOGISTON_FULL "Your light item is full."
#endif

#ifdef JP
#define MES_PREVENT_BY_RIDING "乗馬中には出来ない。"
#define MES_PREVENT_BY_CONFUSION "混乱状態で上手く出来ない。"
#define MES_PREVENT_BY_BLIND "眼が見えなくて出来ない。"
#define MES_PREVENT_BY_NO_LITE "明かりがなくて出来ない。"
#define MES_PREVENT_BY_HALLUCINATION "幻覚に阻まれてできない。"
#define MES_PREVENT_BY_STUNED "頭が朦朧としていて集中できない。"
#define MES_PREVENT_BY_AFRAID "恐怖に怯えてできない"
#define MES_PREVENT_BY_PET "今はペットを操ることに集中していないと。"
#define MES_PREVENT_BY_NO_MIRROR "鏡の上でないと集中できない。"
#define MES_PREVENT_BY_NO_BAREBAND "素手じゃないとできません。"
#define MES_PREVENT_BY_NO_SHOOT "射撃用の武器を持っていない。"
#define MES_PREVENT_BY_NO_BOW "弓を装備していない。"
#define MES_PREVENT_ACTIVATE_ONLY "この武器は発動して使うもののようだ。"
#define MES_PREVENT_BY_HP "HPが足りません."
#define MES_PREVENT_BY_MP "MPが足りません。"
#define MES_PREVENT_BY_LEVEL "この能力を使用するにはレベル %d に達していなければなりません。"
#define MES_PREVENT_BY_ANTIMAGIC "反魔法バリアが魔法を邪魔した！"
#define MES_PREVENT_BY_TIMESTOP "止まった時の中ではうまく働かないようだ。"
#define MES_PREVENT_BY_HUNGER "その前に食事をとらないと。"
#define MES_PREVENT_BY_POSTURE "今は構えに集中している。"
#define MES_PREVENT_BY_CREATURE(NAME) "%sが邪魔だ。", (NAME)
#define MES_PREVENT_FISHING_BY_FEATURE "そこは水辺ではない。"
#define MES_PREVENT_MAGIC_BY_DUNGEON "ダンジョンが魔法を吸収した！"
#define MES_PREVENT_LITE_BY_DUNGEON "ダンジョンが光を吸収した。"
#define MES_PREVENT_TELEPORT_BY_CREATURE "テレポートを邪魔された！"
#define MES_PREVENT_QUAFF_BY_TIMESTOP "瓶から液体が流れ出てこない！"
#else
#define MES_PREVENT_BY_RIDING "You cannot do it when riding."
#define MES_PREVENT_BY_CONFUSION "You are too confused."
#define MES_PREVENT_BY_BLIND "You are blind!"
#define MES_PREVENT_BY_NO_LITE "You have no light to read by."
#define MES_PREVENT_BY_HALLUCINATION "You are too hallucinated!"
#define MES_PREVENT_BY_STUNED "You are too stuned!"
#define MES_PREVENT_BY_AFRAID "You are trembling with fear."
#define MES_PREVENT_BY_PET "You need concentration on the pets now."
#define MES_PREVENT_BY_NO_MIRROR "Here are not any mirrors!"
#define MES_PREVENT_BY_NO_BAREBAND "You need to be bare hand."
#define MES_PREVENT_BY_NO_SHOOT "You have nothing to fire with."
#define MES_PREVENT_BY_NO_BOW "You wield no bow!"
#define MES_PREVENT_ACTIVATE_ONLY "Do activate."
#define MES_PREVENT_BY_HP "You do not have enough hit-point to use this power."
#define MES_PREVENT_BY_MP "You do not have enough mana to use this power."
#define MES_PREVENT_BY_LEVEL "You need to attain level %d to use this power."
#define MES_PREVENT_BY_ANTIMAGIC "An anti-magic shell disrupts your magic!"
#define MES_PREVENT_BY_TIMESTOP "It shows no reaction."
#define MES_PREVENT_BY_HUNGER "You must eat something here."
#define MES_PREVENT_BY_POSTURE "You need concentration on your form."
#define MES_PREVENT_BY_CREATURE(NAME) "%^s is stand in your way.", (NAME)
#define MES_PREVENT_MAGIC_BY_DUNGEON "This spell can only be used on the surface!"
#define MES_PREVENT_FISHING_BY_FEATURE "There is no fishing place."
#define MES_PREVENT_LITE_BY_DUNGEON "The darkness of this dungeon absorb your light."
#define MES_PREVENT_TELEPORT_BY_CREATURE "Your teleportation is blocked!"
#define MES_PREVENT_QUAFF_BY_TIMESTOP "The potion doesn't flow out from a bottle."
#endif

#ifdef JP
#define MES_CLOSE_NO_TARGET "そこには閉じるものが見当たらない。"
#define MES_DOOR_STUCK "何かがつっかえて閉まらない。"
#define MES_DOOR_BROKEN "扉は壊れてしまっている。"
#else
#define MES_CLOSE_NO_TARGET "You see nothing there to close."
#define MES_DOOR_STUCK "There seems stuck."
#define MES_DOOR_BROKEN "The door appears to be broken."
#endif

#ifdef JP
#define MES_GAINMAGIC_LIST "[A] 杖, [B] 魔法棒, [C] ロッド:"
#define MES_GAINMAGIC_NO_TYPE "その種類の魔法は覚えていない。"
#define MES_GAINMAGIC_CHARGING "その魔法はまだ充填している最中だ。"
#define MES_GAINMAGIC_NO_LEFT "その魔法は使用回数が切れている。"
#define MES_GAINMAGIC_WHICH_ITEM "どのアイテムの魔力を取り込みますか? "
#define MES_GAINMAGIC_NO_ITEM "魔力を取り込めるアイテムがない。"
#define MES_GAINMAGIC_NO_MAGIC "この杖には発動の為の能力は何も備わっていないようだ。"
#define MES_GAINMAGIC_NEED_IDENTIFY "鑑定されていないと取り込めない。"
#define MES_GAINMAGIC_STILL_CHARGING "充填中のアイテムは取り込めない。"
#define MES_GAINMAGIC_ABSORBED(OBJECT) "%sの魔力を取り込んだ。", (OBJECT)
#define MES_GAINMAGIC_NONE "魔法を覚えていない！"
#else
#define MES_GAINMAGIC_LIST "[A] staff, [B] wand, [C] rod:"
#define MES_GAINMAGIC_NO_TYPE "You don't have that type of magic."
#define MES_GAINMAGIC_CHARGING "The magic are still charging."
#define MES_GAINMAGIC_NO_LEFT "The magic has no charges left."
#define MES_GAINMAGIC_WHICH_ITEM "Gain power of which item? "
#define MES_GAINMAGIC_NO_ITEM "You have nothing to gain power."
#define MES_GAINMAGIC_NO_MAGIC "This staff doesn't have any magical ability."
#define MES_GAINMAGIC_NEED_IDENTIFY "You need to identify before absorbing."
#define MES_GAINMAGIC_STILL_CHARGING "This item is still charging."
#define MES_GAINMAGIC_ABSORBED(OBJECT) "You absorb magic of %s.", (OBJECT)
#define MES_GAINMAGIC_NONE "You don't have any magic!"
#endif

#ifdef JP
#define MES_VISUAL_COL_SYM(COL, SYM) "現在値  色 / 文字 = %3d / %3d", (COL), (SYM)
#define MES_VISUAL_RESET "画面上の[色/文字]を初期値にリセットしました。"
#else
#define MES_VISUAL_COL_SYM(COL, SYM) "Current attr/char = %3d / %3d", (COL), (SYM)
#define MES_VISUAL_RESET "Visual attr/char tables reset."
#endif

#ifdef JP
#define MES_CR_DESC_RIDING "(乗馬中)"
#define MES_CR_DESC_CHAMELEON "(カメレオン)"
#define MES_CR_DESC_CHAMELEON_LORD "(カメレオンの王)"
#else
#define MES_CR_DESC_RIDING "(Riding)"
#define MES_CR_DESC_CHAMELEON "(Chameleon)"
#define MES_CR_DESC_CHAMELEON_LORD "(Chameleon Lord)"
#endif

#ifdef JP
#define MES_OBJ_DESC_TO_SPEED "加速"
#define MES_OBJ_DESC_ATTACK "攻撃"
#define MES_OBJ_DESC_STEALTH "隠密"
#define MES_OBJ_DESC_SEARCH "探索"
#define MES_OBJ_DESC_INFRA "赤外線視力"
#else
#define MES_OBJ_DESC_TO_SPEED "to speed"
#define MES_OBJ_DESC_ATTACK " attack"
#define MES_OBJ_DESC_STEALTH " to stealth"
#define MES_OBJ_DESC_SEARCH " to searching"
#define MES_OBJ_DESC_INFRA " to infravision"
#endif

#ifdef JP
#define MES_INN_FULL "今は満腹だ。"
#define MES_INN_FOOD "バーテンはいくらかの食べ物とビールをくれた。"
#define MES_INN_CANCEL1 "あなたに必要なのは部屋ではなく、治療者です。"
#define MES_INN_CANCEL2 "すみません、でもうちで誰かに死なれちゃ困りますんで。"
#define MES_INN_NIGHTMARE "あなたは絶叫して目を覚ました。"
#else
#define MES_INN_FULL "You are full now."
#define MES_INN_FOOD "The barkeep gives you some gruel and a beer."
#define MES_INN_CANCEL1 "You need a healer, not a room."
#define MES_INN_CANCEL2 "Sorry, but don't want anyone dying in here."
#define MES_INN_NIGHTMARE "You awake screaming."
#endif

#ifdef JP
#define MES_RESEARCH_FULL_LIST "全クリーチャーのリスト"
#define MES_RESEARCH_UNIQUE_LIST "ユニーク・クリーチャーのリスト"
#define MES_RESEARCH_NON_UNIQUE_LIST "非ユニーク・クリーチャーのリスト"
#else
#define MES_RESEARCH_FULL_LIST "Full creature list."
#define MES_RESEARCH_UNIQUE_LIST "Unique creature list."
#define MES_RESEARCH_NON_UNIQUE_LIST "Non-unique creature list."
#endif

#ifdef JP
#define MES_HEX_WHICH_CURSE "どれを呪いますか？"
#define MES_HEX_WHICH_NO_WEAPON "武器を装備していない。"
#define MES_HEX_WHICH_NO_ARMOUR "防具を装備していない。"
#define MES_HEX_START_PATIENT "じっと耐えることにした。"
#define MES_HEX_ALREADY_PATIENT "すでに我慢をしている。"
#define MES_HEX_REVENGE_DONE "我慢が解かれた！"
#define MES_HEX_WIZ_REVENGE(POWER) "%d点のダメージを返した。", (POWER)
#else
#define MES_HEX_WHICH_CURSE "Which weapon do you curse?"
#define MES_HEX_WHICH_NO_WEAPON "You wield no weapons."
#define MES_HEX_WHICH_NO_ARMOUR "You wield no piece of armours."
#define MES_HEX_ALREADY_PATIENT "You are already patienting."
#define MES_HEX_START_PATIENT "You decide to patient all damages."
#define MES_HEX_REVENGE_DONE "Time for end of patioence!"
#define MES_HEX_WIZ_REVENGE(POWER) "You return %d damages.", (POWER)
#endif

#ifdef JP
#define MES_AVTIVATE_FAILED "うまく始動させることができなかった。"
#define MES_AVTIVATE_TIMEOUT "それは微かに音を立て、輝き、消えた..."
#define MES_AVTIVATE_DONE "始動させた..."
#else
#define MES_AVTIVATE_FAILED "You failed to activate it properly."
#define MES_AVTIVATE_TIMEOUT "It whines, glows and fades..."
#define MES_AVTIVATE_DONE "You activate it..."
#endif

#ifdef JP
#define MES_TUNNEL_NONE "そこには何も見当たらない。"
#define MES_TUNNEL_NO_TARGET "そこには掘るものが見当たらない。"
#else
#define MES_TUNNEL_NONE "You see nothing there."
#define MES_TUNNEL_NO_TARGET "You see nothing there to tunnel."
#endif

#ifdef JP
#define MES_EAT_LESS_EFFECT_BY_VAMPIRE "あなたのような者にとって食糧など僅かな栄養にしかならない。"
#define MES_EAT_LESS_EFFECT_BY_VAMPIRE2 "あなたの飢えは新鮮な血によってのみ満たされる！"
#define MES_EAT_LESS_EFFECT_MORTAL_FOOD "生者の食物はあなたにとってほとんど栄養にならない。"
#else
#define MES_EAT_LESS_EFFECT_BY_VAMPIRE "Mere victuals hold scant sustenance for a being such as yourself."
#define MES_EAT_LESS_EFFECT_BY_VAMPIRE2 "Your hunger can only be satisfied with fresh blood!"
#define MES_EAT_LESS_EFFECT_MORTAL_FOOD "The food of mortals is poor sustenance for you."
#endif

#ifdef JP
#define MES_COMBATOP_WEAPON_ONE_HANDED "武器を片手で持つ"
#define MES_COMBATOP_WEAPON_TWO_HANDED "武器を両手で持つ"
#define MES_COMBATOP_BAREHAND_ONE_HANDED "片手で格闘する"
#define MES_COMBATOP_BAREHAND_TWO_HANDED "両手で格闘する"
#define MES_COMBATOP_BAREHAND_ON "格闘を行う"
#define MES_COMBATOP_BAREHAND_OFF "格闘を行わない"
#else
#define MES_COMBATOP_WEAPON_ONE_HANDED "use one hand to control a riding pet"
#define MES_COMBATOP_WEAPON_TWO_HANDED "use both hands for a weapon"
#define MES_COMBATOP_BAREHAND_ONE_HANDED "use one hand to control a riding pet"
#define MES_COMBATOP_BAREHAND_TWO_HANDED "use both hands for melee"
#define MES_COMBATOP_BAREHAND_ON "use one hand for melee"
#define MES_COMBATOP_BAREHAND_OFF "use one hand to control a riding pet"
#endif

#ifdef JP
#define MES_FOOD_WHICH_EAT "どれを食べますか? "
#define MES_FOOD_NO_EAT "食べ物がない。"
#define MES_FOOD_BISCUIT "甘くてサクサクしてとてもおいしい。"
#define MES_FOOD_RATION "これはおいしい。"
#define MES_FOOD_JERKY "歯ごたえがあっておいしい。"
#define MES_FOOD_SLIME_MOLD "これはなんとも形容しがたい味だ。"
#define MES_FOOD_WAYBREAD "これはひじょうに美味だ。"
#define MES_FOOD_DRINK "のどごし爽やかだ。"
#define MES_FOOD_SKELETON_FAILED1 "食べ物がアゴを素通りして落ちた！"
#define MES_FOOD_SKELETON_FAILED2 "食べ物がアゴを素通りして落ち、消えた！"
#else
#define MES_FOOD_WHICH_EAT "Eat which item? "
#define MES_FOOD_NO_EAT "You have nothing to eat."
#define MES_FOOD_BISCUIT "That tastes good."
#define MES_FOOD_RATION "That tastes good."
#define MES_FOOD_JERKY "That tastes good."
#define MES_FOOD_SLIME_MOLD "That tastes good."
#define MES_FOOD_WAYBREAD "That tastes good."
#define MES_FOOD_DRINK "That tastes good."
#define MES_FOOD_SKELETON_FAILED1 "The food falls through your jaws!"
#define MES_FOOD_SKELETON_FAILED2 "The food falls through your jaws and vanishes!"
#endif

#ifdef JP
#define MES_QUAFF_WHICH_POTION "どの薬を飲みますか? "
#define MES_QUAFF_NO_POTION "飲める薬がない。"
#define MES_QUAFF_WATER "口の中がさっぱりした。"
#define MES_QUAFF_APPLE_JUICE "甘くてサッパリとしていて、とてもおいしい。"
#define MES_QUAFF_LESS_THIRSTY "のどの渇きが少しおさまった。"
#define MES_QUAFF_DRUNKER1 "知らない場所で目が醒めた。頭痛がする。"
#define MES_QUAFF_DRUNKER2 "何も思い出せない。どうやってここへ来たのかも分からない！"
#define MES_EFFECT_LOST_MEMORY "過去の記憶が薄れていく気がする。"
#define MES_QUAFF_SKELETON_EFFECT "液体の一部はあなたのアゴを素通りして落ちた！"
#define MES_QUAFF_FLASK "オイルを補給した。"
#define MES_QUAFF_MOIST "水分を取り込んだ。"
#else
#define MES_QUAFF_WHICH_POTION "Quaff which potion? "
#define MES_QUAFF_NO_POTION "You have no potions to quaff."
#define MES_QUAFF_WATER ""
#define MES_QUAFF_APPLE_JUICE ""
#define MES_QUAFF_LESS_THIRSTY "You feel less thirsty."
#define MES_QUAFF_DRUNKER1 "You wake up somewhere with a sore head..."
#define MES_QUAFF_DRUNKER2 "You can't remember a thing, or how you got here!"
#define MES_EFFECT_LOST_MEMORY "You feel your memories fade."
#define MES_QUAFF_SKELETON_EFFECT "Some of the fluid falls through your jaws!"
#define MES_QUAFF_FLASK "You replenish yourself with the oil."
#define MES_QUAFF_MOIST "You are moistened."
#endif

#ifdef JP
#define MES_SCROLL_WHICH_READ "どの巻物を読みますか? "
#define MES_SCROLL_NO_READ "読める巻物がない。"
#define MES_SCROLL_RUMOR_DEFAULT "嘘の噂もある。"
#define MES_SCROLL_DISABLE "巻物なんて読めない。"
#define MES_SCROLL_USED "巻物は煙を立てて消え去った！"
#define MES_SCROLL_MESSAGE "巻物にはメッセージが書かれている:"
#else
#define MES_SCROLL_WHICH_READ "Read which scroll? "
#define MES_SCROLL_NO_READ "You have no scrolls to read."
#define MES_SCROLL_RUMOR_DEFAULT "Some rumors are wrong."
#define MES_SCROLL_DISABLE "You cannot read."
#define MES_SCROLL_USED "The scroll disappears in a puff of smoke!"
#define MES_SCROLL_MESSAGE "There is message on the scroll. It says:"
#endif

#ifdef JP
#define MES_GAMBLE_BETWEEN "イン・ビトイーン"
#define MES_GAMBLE_CRAPS "クラップス"
#define MES_GAMBLE_WHEEL "ルーレット"
#define MES_GAMBLE_DICESLOTS "ダイス・スロット"
#define MES_GAMBLE_CRAPS_FIRST(ROLL1, ROLL2, ROLL3) "１振りめ: %d %d      Total: %d", (ROLL1), (ROLL2), (ROLL3)
#define MES_GAMBLE_CRAPS_RESULT(ROLL1, ROLL2, ROLL3) "出目: %d %d          合計:      %d", (ROLL1), (ROLL2), (ROLL3)
#define MES_GAMBLE_ARENA_WHICH "どれに賭けますか:"
#define MES_GAMBLE_ARENA_ODDS_LIST "クリーチャー                                                    倍率"
#define MES_GAMBLE_ARENA_ODDS_LIST2(NUM, SPECIES, ODDS) "%d) %-58s  %4ld.%02ld倍", (NUM)+1, format("%s%s",(SPECIES)->name + species_name, has_trait_species((SPECIES), TRAIT_UNIQUE) ? "もどき" : "      "), (ODDS)/100, (ODDS)%100
#define MES_GAMBLE_WON "あなたの勝ち"
#define MES_GAMBLE_LOST "あなたの負け"
#define MES_GAMBLE_TOTAL_WON "「今回は儲けたな！でも次はこっちが勝ってやるからな、絶対に！」"
#define MES_GAMBLE_TOTAL_LOST "「金をスッてしまったな、わはは！うちに帰った方がいいぜ"
#define MES_GAMBLE_PAYOFF(ODDS) "倍率: %d", (ODDS)
#define MES_GAMBLE_AGAIN "もう一度(Y/N)？"
#define MES_GAMBLE_BEFORE_MONEY(MONEY) "ゲーム前の所持金: %9ld", (MONEY)
#define MES_GAMBLE_CURRENT_MONEY(MONEY) "現在の所持金:     %9ld", (MONEY)
#define MES_GAMBLE_CURRENT_WAGER(MONEY) "現在の掛け金:     %9ld", (MONEY)
#define MES_GAMBLE_ROLE_AGAIN "なにかキーを押すともう一回振ります。"
#define MES_GAMBLE_BET(BET) "賭け金 (1-%ld)？", (BET)
#define MES_GAMBLE_BET_LIMIT(MONEY) "%ldゴールドだけ受けよう。残りは取っときな。", (MONEY)
#define MES_GAMBLE_BET_UNDERLINE "ＯＫ、1ゴールドでいこう。"
#define MES_GAMBLE_BET_OUT_NUM1 "0番にしとくぜ。"
#define MES_GAMBLE_BET_OUT_NUM2 "ＯＫ、9番にしとくぜ。"
#define MES_GAMBLE_WHEEL_WHICH_NUM "何番？ (0-9)"
#define MES_GAMBLE_ROULETTE_RESULT(NUM) "ルーレットは回り、止まった。勝者は %d番だ。", (NUM)
#else
#define MES_GAMBLE_BETWEEN "In Between"
#define MES_GAMBLE_CRAPS "Craps"
#define MES_GAMBLE_WHEEL "Wheel"
#define MES_GAMBLE_DICESLOTS "Dice Slot"
#define MES_GAMBLE_CRAPS_FIRST(ROLL1, ROLL2, ROLL3) "First roll: %d %d    Total: %d", (ROLL1), (ROLL2), (ROLL3)
#define MES_GAMBLE_CRAPS_RESULT(ROLL1, ROLL2, ROLL3) "Roll result: %d %d   Total:     %d", (ROLL1), (ROLL2), (ROLL3)
#define MES_GAMBLE_ARENA_WHICH "Which creature: "
#define MES_GAMBLE_ARENA_ODDS_LIST "Creatures                                                       Odds"
#define MES_GAMBLE_ARENA_ODDS_LIST2(NUM, SPECIES, ODDS) "%d) "%d) %-58s  %4ld.%02ld", (NUM)+1, format("%s%s",(SPECIES)->name + species_name, has_trait_species(species_ptr, TRAIT_UNIQUE) ? "Fake " : ""), (ODDS)/100, (ODDS)%100
#define MES_GAMBLE_WON "YOU WON"
#define MES_GAMBLE_LOST "You Lost"
#define MES_GAMBLE_TOTAL_WON "You came out a winner! We'll win next time, I'm sure."
#define MES_GAMBLE_TOTAL_LOST "You lost gold! Haha, better head home."
#define MES_GAMBLE_PAYOFF(ODDS) "Payoff: %d", (ODDS)
#define MES_GAMBLE_AGAIN "Again(Y/N)?"
#define MES_GAMBLE_BEFORE_MONEY(MONEY) "Gold before game: %9ld", (MONEY)
#define MES_GAMBLE_CURRENT_MONEY(MONEY) "Current Gold:     %9ld", (MONEY)
#define MES_GAMBLE_CURRENT_WAGER(MONEY) "Current Wager:    %9ld", (MONEY)
#define MES_GAMBLE_ROLE_AGAIN "Hit any key to roll again"
#define MES_GAMBLE_BET(BET) "Your wager (1-%ld) ? ", (BET)
#define MES_GAMBLE_BET_LIMIT(MONEY) "I'll take %ld gold of that. Keep the rest.", (MONEY)
#define MES_GAMBLE_BET_UNDERLINE "Ok, we'll start with 1 gold."
#define MES_GAMBLE_BET_OUT_NUM1 "I'll put you down for 0."
#define MES_GAMBLE_BET_OUT_NUM2 "Ok, I'll put you down for 9."
#define MES_GAMBLE_WHEEL_WHICH_NUM "Pick a number (0-9)"
#define MES_GAMBLE_ROULETTE_RESULT(NUM) "The wheel spins to a stop and the winner is %d", (NUM)
#endif

#ifdef JP
#define MES_INFO_UNKNOWN_ARTIFACT "未知のアーティファクト"
#define MES_INFO_KNOWN_ARTIFACT "既知のアーティファクト"
#define MES_INFO_KILL_COUNT "殺したクリーチャーの数"
#define MES_INFO_BOUNTY "賞金首の一覧"
#define MES_INFO_BOUNTY_NONE "賞金首はもう残っていません。"
#define MES_INFO_DUNGEON "今までに入ったダンジョン"
#define MES_INFO_SELF "自分に関する情報"
#define MES_INFO_QUEST "クエスト達成状況"
#define MES_INFO_HOME "我が家のアイテム"
#define MES_INFO_AUTOPICK "自動拾い/破壊 設定リスト"
#define MES_INFO_AUTOPICK_NO_LINE "自動破壊/拾いには何も登録されていません。"
#define MES_INFO_AUTOPICK_LINE(NUM) "   自動拾い/破壊には現在 %d行登録されています。\n\n", (NUM)
#else
#define MES_INFO_UNKNOWN_ARTIFACT "Unknown Artifact"
#define MES_INFO_KNOWN_ARTIFACT "Artifacts Seen"
#define MES_INFO_KILL_COUNT "Kill Count"
#define MES_INFO_BOUNTY "Wanted creatures"
#define MES_INFO_BOUNTY_NONE "There is no more wanted creature."
#define MES_INFO_DUNGEON "Dungeon"
#define MES_INFO_SELF "HP-rate & Max stat"
#define MES_INFO_QUEST "Quest status"
#define MES_INFO_HOME "Home inventory"
#define MES_INFO_AUTOPICK "Auto-picker/Destroyer"
#define MES_INFO_AUTOPICK_NO_LINE "No preference for auto picker/destroyer."
#define MES_INFO_AUTOPICK_LINE(NUM) "   There are %d registered lines for auto picker/destroyer.\n\n", (NUM)
#endif

#ifdef JP
#define MES_QUERY_ALL_CREATURE "全クリーチャーのリスト"
#define MES_QUERY_UNIQUE "ユニーク・クリーチャーのリスト"
#define MES_QUERY_NON_UNIQUE "ユニーク外クリーチャーのリスト"
#define MES_QUERY_STEED "乗馬可能クリーチャーのリスト"
#define MES_QUERY_INPUT_SYM "知りたい文字を入力して下さい(記号 or ^A全,^Uユ,^N非ユ,^R乗馬,^M名前): "
#define MES_QUERY_INPUT_ENTER_NAME "名前(英語の場合小文字で可)"
#define MES_QUERY_MARCH(KEYWORD) "名前:%sにマッチ", (KEYWORD)
#define MES_QUERY_ASK_RECALL "思い出を見ますか? (k:殺害順/y/n): "
#define MES_QUERY_INTERFACE_RECALL "['r'思い出, ESC]"
#else
#define MES_QUERY_ALL_CREATURE "Full creature list."
#define MES_QUERY_UNIQUE "Unique creature list."
#define MES_QUERY_NON_UNIQUE "Non-unique creature list."
#define MES_QUERY_STEED "Ridable creature list."
#define MES_QUERY_INPUT_SYM "Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): "
#define MES_QUERY_INPUT_ENTER_NAME "Enter name:"
#define MES_QUERY_MARCH(KEYWORD) "Creatures with a name \"%s\"", (KEYWORD)
#define MES_QUERY_ASK_RECALL "Recall details? (k/y/n): "
#define MES_QUERY_INTERFACE_RECALL "[(r)ecall, ESC]"
#endif

#ifdef JP
#define MES_BECOME_BRAND_SHARPNESS "は鋭さを増した！"
#define MES_BECOME_BRAND_EARTHQUAKE "は破壊力を増した！"
#define MES_BECOME_BRAND_SLAYHUMAN "は人間の血を求めている！"
#define MES_BECOME_BRAND_ELEC "は電撃に覆われた！"
#define MES_BECOME_BRAND_ACID "は酸に覆われた！"
#define MES_BECOME_BRAND_SLAYEVIL "は邪悪な者への裁きを求めている！"
#define MES_BECOME_BRAND_SLAYDEMON "は異世界の住人の肉体を求めている！"
#define MES_BECOME_BRAND_SLAYUNDEAD "は不死者を求めている！"
#define MES_BECOME_BRAND_SLAYANIMAL "は動物の血を求めている！"
#define MES_BECOME_BRAND_SLAYDRAGON "はドラゴンの血を求めている！"
#define MES_BECOME_BRAND_SLAYTROLL "はトロルの血を求めている！"
#define MES_BECOME_BRAND_SLAYORC "はオークの血を求めている！"
#define MES_BECOME_BRAND_SLAYGIANT "は巨人の血を求めている！"
#define MES_BECOME_BRAND_TRUMP "は非常に不安定になったようだ。"
#define MES_BECOME_BRAND_VAMPIRIC "は血を求めている！"
#define MES_BECOME_BRAND_POIS "は毒に覆われた。"
#define MES_BECOME_BRAND_CHAOS "は純ログルスに飲み込まれた。"
#define MES_BECOME_BRAND_FIRE "は炎のシールドに覆われた！"
#define MES_BECOME_BRAND_COLD "は深く冷たいブルーに輝いた！"
#define MES_BECOME_BRAND_FORMAT(OBJECT, BRAND) "あなたの%s%s", (OBJECT), (BRAND)
#else
#define MES_BECOME_BRAND_SHARPNESS "becomes very sharp!"
#define MES_BECOME_BRAND_EARTHQUAKE "seems very powerful."
#define MES_BECOME_BRAND_SLAYHUMAN "seems to be looking for humans!"
#define MES_BECOME_BRAND_ELEC "covered with lightning!"
#define MES_BECOME_BRAND_ACID "coated with acid!"
#define MES_BECOME_BRAND_SLAYEVIL "seems to be looking for evil creatures!"
#define MES_BECOME_BRAND_SLAYDEMON "seems to be looking for demons!"
#define MES_BECOME_BRAND_SLAYUNDEAD "seems to be looking for undead!"
#define MES_BECOME_BRAND_SLAYANIMAL "seems to be looking for animals!"
#define MES_BECOME_BRAND_SLAYDRAGON "seems to be looking for dragons!"
#define MES_BECOME_BRAND_SLAYTROLL "seems to be looking for troll!s"
#define MES_BECOME_BRAND_SLAYORC "seems to be looking for orcs!"
#define MES_BECOME_BRAND_SLAYGIANT "seems to be looking for giants!"
#define MES_BECOME_BRAND_TRUMP "seems very unstable now."
#define MES_BECOME_BRAND_VAMPIRIC "thirsts for blood!"
#define MES_BECOME_BRAND_POIS "is coated with poison."
#define MES_BECOME_BRAND_CHAOS "is engulfed in raw Logrus!"
#define MES_BECOME_BRAND_FIRE "is covered in a fiery shield!"
#define MES_BECOME_BRAND_COLD "glows deep, icy blue!"
#define MES_BECOME_BRAND_FORMAT(OBJECT, BRAND) "Your %s %s", (OBJECT), (BRAND)
#endif

#ifdef JP
#define MES_FETCH_STAND "自分の足の下にある物は取れません。"
#define MES_FETCH_TOO_FAR "そんなに遠くにある物は取れません！"
#define MES_FETCH_NO_OBJECT "そこには何もありません。"
#define MES_FETCH_OUT_CONTROL "アイテムがコントロールを外れて落ちた。"
#define MES_FETCH_OUT_SIGHT "そこはあなたの視界に入っていません。"
#define MES_FETCH_WALL "そこは壁の向こうです。"
#define MES_FETCH_TOO_HEAVY "そのアイテムは重過ぎます。"
#define MES_FETCH_DONE(OBJECT) "%^sがあなたの足元に飛んできた。", (OBJECT)->name
#else
#define MES_FETCH_STAND "You can't fetch when you're already standing on something."
#define MES_FETCH_TOO_FAR "You can't fetch something that far away!"
#define MES_FETCH_NO_OBJECT "There is no object at this place."
#define MES_FETCH_OUT_CONTROL "The item slips from your control."
#define MES_FETCH_OUT_SIGHT "You have no direct line of sight to that location."
#define MES_FETCH_WALL "You have no direct line of sight to that location."
#define MES_FETCH_TOO_HEAVY "The object is too heavy."
#define MES_FETCH_DONE(OBJECT) "%^s flies through the air to your feet.", (OBJECT)->name
#endif

#ifdef JP
#define MES_ALT_REAL_START "回りの景色が変わり始めた..."
#define MES_ALT_REAL_CANCEL "景色が元に戻った..."
#else
#define MES_ALT_REAL_START "The view around you begins to change..."
#define MES_ALT_REAL_CANCEL "The view around you got back..."
#endif

#ifdef JP
#define MES_DISENCHANT_NO_EFFECT(OBJECT) "しかし%sには効果がなかった！", (OBJECT)->name
#define MES_DISENCHANT_DAMAGED(OBJECT) "%sがダメージを受けた！", (OBJECT)->name
#else
#define MES_DISENCHANT_NO_EFFECT(OBJECT) "Your %s is unaffected!", (OBJECT)->name
#define MES_DISENCHANT_DAMAGED(OBJECT) "Your %s is damaged!", (OBJECT)->name
#endif

#ifdef JP
#define MES_RUSTPROOF_WHICH_OBJECT "どの防具に錆止めをしますか？"
#define MES_RUSTPROOF_NO_OBJECT "錆止めできるものがありません。"
#define MES_RUSTPROOF_AS_NEW(OBJECT) "%sは新品同様になった！", (OBJECT)->name
#define MES_RUSTPROOF_PROTECTED(OBJECT) "%sは腐食しなくなった。", (OBJECT)->name
#else
#define MES_RUSTPROOF_WHICH_OBJECT "Rustproof which piece of armour? "
#define MES_RUSTPROOF_NO_OBJECT "You have nothing to rustproof."
#define MES_RUSTPROOF_AS_NEW(OBJECT) "The %s look%s as good as new!", (OBJECT)->name, (((OBJECT)->number > 1) ? "" : "s")
#define MES_RUSTPROOF_PROTECTED(OBJECT) "The %s %s now protected against corrosion.", (OBJECT)->name, (((OBJECT)->number > 1) ? "are" : "is")
#endif

#ifdef JP
#define MES_DDOOR_FAILED "精霊界から物質界に戻る時うまくいかなかった！"
#define MES_DMIRROR_FAILED "鏡の世界をうまく通れなかった！"
#else
#define MES_DDOOR_FAILED "You fail to exit the astral plane correctly!"
#define MES_DMIRROR_FAILED "You fail to pass the mirror plane correctly!"
#endif

#ifdef JP
#define MES_EATMAGIC_WHICH_ITEM "どのアイテムから魔力を吸収しますか？"
#define MES_EATMAGIC_NO_ITEM "魔力を吸収できるアイテムがありません。"
#define MES_EATMAGIC_DISCHARGED_ROD "充填中のロッドから魔力を吸収することはできません。"
#define MES_EATMAGIC_EMPTY "吸収できる魔力がありません！"
#define MES_EATMAGIC_BACKFIRE(OBJECT) "魔力が逆流した！%sは完全に魔力を失った。", (OBJECT)->name
#define MES_EATMAGIC_LOST_MANA(OBJECT) "%sは破損を免れたが、魔力が全て失われた。", (OBJECT)->name
#else
#define MES_EATMAGIC_WHICH_ITEM "Drain which item? "
#define MES_EATMAGIC_NO_ITEM "You have nothing to drain."
#define MES_EATMAGIC_DISCHARGED_ROD "You can't absorb energy from a discharged rod."
#define MES_EATMAGIC_EMPTY "There's no energy there to absorb!"
#define MES_EATMAGIC_BACKFIRE(OBJECT) "The recharging backfires - %s is completely drained!", (OBJECT)->name
#define MES_EATMAGIC_LOST_MANA(OBJECT) "You save your %s from destruction, but all charges are lost.", (OBEJCT)->name
#endif

#ifdef JP
#define MES_PLEFFILE_TOO_MACRO "マクロトリガーの設定が多すぎます!"
#define MES_PLEFFILE_INVALID_OPTION(NAME) "オプションの名前が正しくありません： %s", (NAME)
#else
#define MES_PLEFFILE_TOO_MACRO "Too many macro triggers!"
#define MES_PLEFFILE_INVALID_OPTION(NAME) "Ignored invalid option: %s", (NAME)
#endif

#ifdef JP
#define MES_CREATURE_LIFE_COND1 "無傷"
#define MES_CREATURE_LIFE_COND2 "軽傷"
#define MES_CREATURE_LIFE_COND3 "負傷"
#define MES_CREATURE_LIFE_COND4 "重症"
#define MES_CREATURE_LIFE_COND5 "半死半生"
#define MES_CREATURE_NO_LIFE_COND1 "無ダメージ"
#define MES_CREATURE_NO_LIFE_COND2 "小ダメージ"
#define MES_CREATURE_NO_LIFE_COND3 "中ダメージ"
#define MES_CREATURE_NO_LIFE_COND4 "大ダメージ"
#define MES_CREATURE_NO_LIFE_COND5 "倒れかけ"
#define MES_CREATURE_LIFERATE_NOW(NUM) "現在の体力ランクは %d/100 です。", (NUM)
#define MES_CREATURE_LIFERATE_CHANGED "体力ランクが変わった。"
#else
#define MES_CREATURE_LIFE_COND1 "unhurt"
#define MES_CREATURE_LIFE_COND2 "somewhat wounded"
#define MES_CREATURE_LIFE_COND3 "wounded"
#define MES_CREATURE_LIFE_COND4 "badly wounded"
#define MES_CREATURE_LIFE_COND5 "almost dead"
#define MES_CREATURE_NO_LIFE_COND1 "undamaged"
#define MES_CREATURE_NO_LIFE_COND2 "somewhat damaged"
#define MES_CREATURE_NO_LIFE_COND3 "damaged"
#define MES_CREATURE_NO_LIFE_COND4 "badly damaged"
#define MES_CREATURE_NO_LIFE_COND5 "almost destroyed"
#define MES_CREATURE_LIFERATE_NOW(NUM) "Your life rate is %d/100 now.", (NUM)
#define MES_CREATURE_LIFERATE_CHANGED "Life rate is changed."
#endif

#ifdef JP
#define MES_SCORE_PUSHKEY "[ ESCで中断, その他のキーで続けます ]"
#define MES_SCORE_ASK_SEND_SCORE "スコアをスコア・サーバに登録しますか? "
#define MES_SCORE_SENDING "送信中．．"
#define MES_SCORE_SEND "完了。何かキーを押してください。"
#define MES_SCORE_DISABLE_WIZARD "ウィザード・モードではスコアが記録されません。"
#define MES_SCORE_DISABLE_UNIQUE "ユニーク・モードではスコアが記録されません。"
#define MES_SCORE_DISABLE_CHEAT "詐欺をやった人はスコアが記録されません。"
#define MES_SCORE_DISABLE_INTERUPT "強制終了のためスコアが記録されません。"
#define MES_SCORE_DISABLE_QUIT "途中終了のためスコアが記録されません。"
#define MES_SCORE_SAVE_DEATH "死んだデータをセーブしますか？ "
#define MES_SCORE_STAND_BY "後でスコアを登録するために待機しますか？"
#define MES_SCORE_REGISTER_NOW "待機していたスコア登録を今行ないますか？"
#define MES_SCORE_REGISTER_CEASE "スコア登録を諦めますか？"
#define MES_SCORE_STAND_BY_NEXT "引き続き待機します。"
#else
#define MES_SCORE_PUSHKEY "[Press ESC to quit, any other key to continue.]"
#define MES_SCORE_ASK_SEND_SCORE "Do you send score to the world score sever? "
#define MES_SCORE_SENDING "Sending..."
#define MES_SCORE_SEND "Completed.  Hit any key."
#define MES_SCORE_DISABLE_WIZARD "Score not registered for wizards."
#define MES_SCORE_DISABLE_UNIQUE "Score not registered for uniques."
#define MES_SCORE_DISABLE_CHEAT "Score not registered for cheaters."
#define MES_SCORE_DISABLE_INTERUPT "Score not registered due to interruption."
#define MES_SCORE_DISABLE_QUIT "Score not registered due to quitting."
#define MES_SCORE_SAVE_DEATH "Save death? "
#define MES_SCORE_STAND_BY "Stand by for later score registration? "
#define MES_SCORE_REGISTER_NOW "Do you register score now? "
#define MES_SCORE_REGISTER_CEASE "Do you give up score registration? "
#define MES_SCORE_STAND_BY_NEXT "standing by for future registration..."
#endif

#ifdef JP
#define MES_WINNER_WON1 "*** おめでとう ***"
#define MES_WINNER_WON2 "あなたはキャンペーンをコンプリートしました。"
#define MES_WINNER_WON3 "いつでも引退(自殺コマンド)することができます。"
#else
#define MES_WINNER_WON1 "*** CONGRATULATIONS ***"
#define MES_WINNER_WON2 "You have won the game!"
#define MES_WINNER_WON3 "You may retire (commit suicide) when you are ready."
#endif

#ifdef JP
#define MES_CHEAT_DEATH "ウィザードモードに念を送り、死を欺いた。"
#else
#define MES_CHEAT_DEATH "You invoke wizard mode and cheat death."
#endif

#ifdef JP
#define MES_QUICKSTART_DESC "クイック・スタートを使うと以前と全く同じキャラクターで始められます。"
#define MES_QUICKSTART_ASK "クイック・スタートを使いますか？[y/N]"
#else
#define MES_QUICKSTART_DESC "Do you want to use the quick start function(same character as your last one)."
#define MES_QUICKSTART_ASK "Use quick start? [y/N]" 
#endif

#ifdef JP
#define MES_TELEPORT_PREVENTED "不思議な力がテレポートを防いだ！"
#define MES_TELEPORT_NO_DUNGEON "選べるダンジョンがない。"
#define MES_TELEPORT_WHICH(NOTE) "どのダンジョン%sしますか:", (NOTE)
#else
#define MES_TELEPORT_PREVENTED "A mysterious force prevents you from teleporting!"
#define MES_TELEPORT_NO_DUNGEON "No dungeon is available."
#define MES_TELEPORT_WHICH(NOTE) "Which dungeon do you %s?: ", (NOTE)
#endif

#ifdef JP
#define MES_CHEST_STAT_EMPTY "(空)"
#define MES_CHEST_STAT_DISARMED "(解除済)"
#define MES_CHEST_STAT_UNLOCKED "(非施錠)"
#define MES_CHEST_STAT_LOCKED "(施錠)"
#define MES_CHEST_STAT_POISON_NEEDLE "(毒針)"
#define MES_CHEST_STAT_GAS_TRAP "(ガス・トラップ)"
#define MES_CHEST_STAT_BOMB "(爆発装置)"
#define MES_CHEST_STAT_SUMMON "(召喚のルーン)"
#define MES_CHEST_STAT_EVIL "(邪悪なルーン)"
#define MES_CHEST_STAT_ALARM "(警報装置)"
#define MES_CHEST_STAT_MULTI "(マルチ・トラップ)"
#else
#define MES_CHEST_STAT_EMPTY "(empty)"
#define MES_CHEST_STAT_DISARMED "(disarmed)"
#define MES_CHEST_STAT_UNLOCKED "(unlocked)"
#define MES_CHEST_STAT_LOCKED "(locked)"
#define MES_CHEST_STAT_POISON_NEEDLE "(poison needle)"
#define MES_CHEST_STAT_GAS_TRAP "(Gas Trap)"
#define MES_CHEST_STAT_BOMB "(Explosion Device)"
#define MES_CHEST_STAT_SUMMON "(Summoning Runes)"
#define MES_CHEST_STAT_EVIL "(Gleaming Black Runes)"
#define MES_CHEST_STAT_ALARM "(Alarm)"
#define MES_CHEST_STAT_MULTI "(Multiple Traps)"
#endif

#ifdef JP
#define MES_WARN_COL_BLACK "黒く"
#define MES_WARN_COL_PURPLE "紫色に"
#define MES_WARN_COL_DEEPRED "紫色に"
#define MES_WARN_COL_RED "赤色に"
#define MES_WARN_COL_PINK "ピンク色に"
#define MES_WARN_COL_WHITE "白く"
#define MES_WARN_GLOW1(OBJECT, COLOR) "%sは%s光った。", (OBJECT)->name, (COLOR)
#define MES_WARN_GLOW2(COLOR) "%s光る物が頭に浮かんだ。", (COLOR)
#else
#define MES_WARN_COL_BLACK "black"
#define MES_WARN_COL_PURPLE "purple"
#define MES_WARN_COL_DEEPRED "deep red"
#define MES_WARN_COL_RED "red"
#define MES_WARN_COL_PINK "pink"
#define MES_WARN_COL_WHITE "white"
#define MES_WARN_GLOW1(OBJECT, COLOR) "%s glows %s.", (OBJECT)->name, (COLOR)
#define MES_WARN_GLOW2(COLOR) "An %s image forms in your mind.", (COLOR)
#endif

