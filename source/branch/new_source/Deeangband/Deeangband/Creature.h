enum CREATURE_STATUS
{
	CS_STR,
	CS_INT,
	CS_WIS,
	CS_DEX,
	CS_CON,
	CS_CHR,
	CS_MAX
};


struct status_table
{
	int current[CS_MAX];
	int max[CS_MAX];
	int max_max[CS_MAX];
};
