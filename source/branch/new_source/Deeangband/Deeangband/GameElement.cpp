/*!
    @file GameElement.cpp
    @brief �Q�[�����v�f�̃X�[�p�[�N���X����
    @date 2014/01/19
    @author Deskull
    2014 Sikabane Works.
*/

#include "stdafx.h"
#include "GameElement.h"

ID GameElement::getID(void)
{
	return this->id;
}

void GameElement::setID(ID newID)
{
	this->id = newID;
}

Tag GameElement::getTag(void)
{
	return this->tag;
}

void GameElement::setTag(Tag newTag)
{
	this->tag = newTag;
}
