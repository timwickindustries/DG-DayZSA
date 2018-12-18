class CfgMods
{
	class KillFeed
	{
	    dir = "KillFeed";
	    picture = "";
	    action = "";
	    hideName = 1;
	    hidePicture = 1;
	    name = "KillFeed";
	    credits = "Cleetus";
	    author = "Cleetus";
	    authorID = "0";
	    version = "1.0";
	    extra = 0;
	    type = "mod";

	    dependencies[] = {"World"};

	    class defs
	    {
			class worldScriptModule
            {
                value = "";
                files[] = {"KillFeed/scripts/4_World"};
            };
        };
    };
};