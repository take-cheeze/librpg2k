namespace rpg2k { namespace define { const char* LcfSaveData =
"array1d Grobal\n"
"{\n"
"	[100]: array1d Preview\n"
"	{\n"
"		[1]: double timeStamp;\n"
"\n"
"		[11]: string name;\n"
"		[12]: int level = 1;\n"
"		[13]: int hp_;\n"
"\n"
"		[21]: string face_set1;\n"
"		[22]: int face_set_pos1 = 0;\n"
"		[23]: string face_set2;\n"
"		[24]: int face_set_pos2 = 0;\n"
"		[25]: string face_set3;\n"
"		[26]: int face_set_pos3 = 0;\n"
"		[27]: string face_set4;\n"
"		[28]: int face_set_pos4 = 0;\n"
"	};\n"
"\n"
"	[101]: array1d System\n"
"	{\n"
"		[1]: int dummy;\n"
"\n"
"		[11]: int dummy;\n"
"\n"
"		[21]: string system;\n"
"		[22]: int wallpaper;\n"
"		[23]: int font;\n"
"\n"
"		[31]: int switch_data_num = 0;\n"
"		[32]: binary switch_data;\n"
"		[33]: int variable_data_num = 0;\n"
"		[34]: binary variable_data;\n"
"\n"
"		[41]: int window_type = 0;\n"
"		[42]: int windowViewPoint = 2;\n"
"		[43]: bool do_notHideParty = true;\n"
"		[44]: bool moveWhenWait = false;\n"
"\n"
"		[51]: string face_set;\n"
"		[52]: int face_set_pos = 0;\n"
"		[53]: int side = 0;\n"
"		[54]: bool flip = false;\n"
"		[55]: bool partyTransparent = false;\n"
"\n"
"		[71]: music dummy;\n"
"		[72]: music battle;\n"
"		[73]: music battleEnd;\n"
"		[74]: music hotel;\n"
"		[75]: music current;\n"
"		[76]: music dummy;\n"
"		[77]: music dummy;\n"
"		[78]: music memorized;\n"
"		[79]: music boat;\n"
"		[80]: music ship;\n"
"		[81]: music airShip;\n"
"		[82]: music gameOver;\n"
"\n"
"		[ 91]: sound cursormove;\n"
"		[ 92]: sound enter;\n"
"		[ 93]: sound cancel;\n"
"		[ 94]: sound buzzer;\n"
"		[ 95]: sound battleStart;\n"
"		[ 96]: sound runAway;\n"
"		[ 97]: sound enemyAttack;\n"
"		[ 98]: sound enemyDamaged;\n"
"		[ 99]: sound partyDamaged;\n"
"		[100]: sound evasion;\n"
"		[101]: sound enamyDead;\n"
"		[102]: sound item_use;\n"
"\n"
"		[111]: binary moveErase;\n"
"		[112]: binary moveShow;\n"
"		[113]: binary battleStartErase;\n"
"		[114]: binary battleStartShow;\n"
"		[115]: binary battleEndErase;\n"
"		[116]: binary battleEndShow;\n"
"\n"
"		[121]: bool canTeleport;\n"
"		[122]: bool canEscape;\n"
"		[123]: bool canSave;\n"
"		[124]: bool canOpenMenu;\n"
"		[125]: string backdrop;\n"
"\n"
"		[131]: int saveTime;\n"
"	};\n"
"\n"
"	[102]: array1d dummy\n"
"	{\n"
"		[1]: binary dummy;\n"
"\n"
"		[11]: binary dummy;\n"
"\n"
"		[31]: binary dummy;\n"
"		[32]: binary dummy;\n"
"\n"
"		[41]: binary dummy;\n"
"		[42]: binary dummy;\n"
"		[43]: binary dummy;\n"
"		[44]: binary dummy;\n"
"		[45]: binary dummy;\n"
"\n"
"		[49]: binary dummy;\n"
"	};\n"
"\n"
"	[103]: array2d Picture\n"
"	{\n"
"		[1]: string picture;\n"
"		[2]: double startX = 160;\n"
"		[3]: double startY = 120;\n"
"		[4]: double currentX = 160;\n"
"		[5]: double currentY = 120;\n"
"		[6]: bool scroll = false;\n"
"		[8]: double dummy;\n"
"		[9]: bool isTrans = false;\n"
"\n"
"		[11]: double red;\n"
"		[12]: double green;\n"
"		[13]: double blue;\n"
"		[14]: double chroma;\n"
"		[15]: int effect;\n"
"		[16]: double speedOrPowerVal;\n"
"\n"
"		[31]: double goalX;\n"
"		[32]: double goalY;\n"
"		[33]: int magnify = 100;\n"
"		[34]: int trans = 0;\n"
"\n"
"		[41]: int goalRed   = 100;\n"
"		[42]: int goalGreen = 100;\n"
"		[43]: int goalBlue  = 100;\n"
"		[44]: int goalChroma = 100;\n"
"		[46]: int speedOrPower;\n"
"\n"
"		[51]: int countDown = 0;\n"
"		[52]: double degree;\n"
"		[53]: int dummy;\n"
"	};\n"
"\n"
"	[104]: event_state Party  ;\n"
"	[105]: event_state Boat   ;\n"
"	[106]: event_state Ship   ;\n"
"	[107]: event_state Airship;\n"
"\n"
"	[108]: array2d character\n"
"	{\n"
"		[1]: string name;\n"
"		[2]: string title;\n"
"		[3]: bool semiTrans;\n"
"\n"
"		[11]: string char_set;\n"
"		[12]: int char_set_pos;\n"
"		[13]: bool trans;\n"
"\n"
"		[21]: string face_set;\n"
"		[22]: int face_set_pos;\n"
"\n"
"		[31]: int level;\n"
"		[32]: int exp;\n"
"		[33]: int hp_;\n"
"		[34]: int mp_;\n"
"\n"
"		[41]: int attack = 0;\n"
"		[42]: int gaurd  = 0;\n"
"		[43]: int mind   = 0;\n"
"		[44]: int speed  = 0;\n"
"\n"
"		[51]: int skill_data_num = 0;\n"
"		[52]: binary skill_data; // std::set<uint16t>\n"
"\n"
"		[61]: binary equipment; // std::array<uint16t, 5>\n"
"\n"
"		[71]: int cur_hp_ = 0;\n"
"		[72]: int cur_mp_ = 0;\n"
"\n"
"		[81]: int conditionStep_num = 0;\n"
"		[82]: binary conditionStep; // std::vector<uint16t>\n"
"		[83]: int condition_num = 0;\n"
"		[84]: binary condition; // std::vector<uint8t>\n"
"\n"
"		[90]: int job;\n"
"		[91]: bool doubleHand;\n"
"	};\n"
"\n"
"	[109]: array1d Status\n"
"	{\n"
"		[1]: int member_num = 0;\n"
"		[2]: binary member;\n"
"\n"
"		[11]: int item_type_num = 0;\n"
"		[12]: binary item_id;\n"
"		[13]: binary item_num;\n"
"		[14]: binary item_use;\n"
"\n"
"		[21]: int money = 0;\n"
"		// [23]:\n"
"\n"
"		[32]: int battle = 0;\n"
"		[33]: int loss = 0;\n"
"		[34]: int win = 0;\n"
"		[35]: int escape = 0;\n"
"\n"
"		[41]: int dummy;\n"
"		[42]: binary dummy;\n"
"	};\n"
"\n"
"	[110]: array2d Teleport\n"
"	{\n"
"		[1]: int map_id = 0;\n"
"		[2]: int x = 0;\n"
"		[3]: int y = 0;\n"
"		[4]: bool onAfterTeleport = false;\n"
"		[5]: int switch_id = 1;\n"
"	};\n"
"\n"
"	[111]: array1d event_data\n"
"	{\n"
"		[1]: int dummy;\n"
"		[3]: int encountRate;\n"
"\n"
"		[5]: int chip_set_id;\n"
"\n"
"		[11]: array2d info = event_state;\n"
"\n"
"		[21]: binary replaceLower;\n"
"		[22]: binary replace_upper;\n"
"\n"
"		[31]: bool usePanorama;\n"
"		[32]: string panorama;\n"
"		[33]: bool horizontalScroll;\n"
"		[34]: bool verticalScroll;\n"
"		[35]: bool horizontalScrollAuto;\n"
"		[36]: int horizontalScrollSpeed;\n"
"		[37]: bool verticalScrollAuto;\n"
"		[38]: int verticalScrollSpeed;\n"
"	};\n"
"\n"
"	[112]: array1d dummy\n"
"	{\n"
"	};\n"
"\n"
"	[113]: array1d dummy\n"
"	{\n"
"		[1]: array2d dummy\n"
"		{\n"
"			[1]: int eventLength;\n"
"			[2]: event dummy;\n"
"\n"
"			[11]: int dummy;\n"
"			[12]: int dummy;\n"
"			[13]: event dummy;\n"
"\n"
"			[21]: int dummy;\n"
"			[22]: int dummy;\n"
"\n"
"			// [31]:\n"
"		};\n"
"		[4]: int dummy;\n"
"	};\n"
"\n"
"	[114]: array2d dummy\n"
"	{\n"
"		[1]: array1d dummy\n"
"		{\n"
"			[1]: binary dummy;\n"
"		};\n"
"	};\n"
"\n"
"};\n"
;
} }
