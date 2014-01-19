/*!
    @file Item.h
    @brief ƒQ[ƒ€’†‚ÌƒAƒCƒeƒ€‚Æ•t—v‘f‚Ìƒvƒƒgƒ^ƒCƒv
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

#include "stdafx.h"
#include "Effect.h"
#include "GameElement.h"

/*! @class Item
    @brief ƒAƒCƒeƒ€‚ÌƒNƒ‰ƒX
*/
class Item : public GameElement
{
private:
	Weight weight; //!< ƒAƒCƒeƒ€‚Ìd—Ê
	AC baseAC; //!< Šî–{‘•bC³
	EV baseEV; //!< Šî–{‰ñ”ğC³
	VO baseVO; //!< Šî–{ˆÓuC³
	AC plusAC; //!< ’Ç‰Á‘•bC³
	EV plusEV; //!< ’Ç‰Á‰ñ”ğC³
	VO plusVO; //!< ’Ç‰ÁˆÓuC³
public:
	
	/*! ó‚¯‚½Œø‰Ê‚ğˆ—‚·‚é */
	bool Item::take_effect(Effect *effectPtr, POWER amount);

	/*! Šî–{‘•bC³‚ğ•Ô‚· */
	AC getBaseAC(void);

	/*! ’Ç‰Á‘•bC³‚ğ•Ô‚· */
	AC getPlusAC(void);

	/*! Šî–{‰ñ”ğC³‚ğ•Ô‚· */
	EV getBaseEV(void);

	/*! ’Ç‰Á‰ñ”ğC³‚ğ•Ô‚· */
	EV getPlusEV(void);

	/*! Šî–{ˆÓuC³‚ğ•Ô‚· */
	VO getBaseVO(void);

	/*! ’Ç‰ÁˆÓuC³‚ğ•Ô‚· */
	VO getPlusVO(void);

};
