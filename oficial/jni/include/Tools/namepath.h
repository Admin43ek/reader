
#include<string.h>

bool baozinb(char *gname, char **name) {
     //ALOGD("判断类名%s",ClassName);        
    if (strstr(gname, "PickUpListWrapperActor") != nullptr)//敌人盒子
    {
        *name = "敌人盒子";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "PlayerDeadBox") != nullptr)//敌人盒子
    {
        *name = "敌人盒子";
        //*color = redcolor;
        return true;
    }
    return false;
}

bool baozine(char *gname, char **name) {
     //ALOGD("判断类名%s",ClassName);        
    if (strstr(gname, "AirDropPlane") != nullptr)//敌人盒子
    {
        *name = "空投飞机";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "AirDropListWrapperActor") != nullptr)//敌人盒子
    {
        *name = "空投盒子";
        //*color = redcolor;
        return true;
    }
    return false;
}



bool baozinbq(char *gname, char **name) {
     //ALOGD("判断类名%s",ClassName);        
    if (strstr(gname, "_Rifle_SCAR_Wrapper_C") != nullptr)
    {
        *name = "SCAR";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "_Rifle_M416_Wrapper_C") != nullptr)
    {
        *name = "M416";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "_MachineGun_UMP9_Wrapper_C") != nullptr)
    {
        *name = "UMP9";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "_MachineGun_P50_Wrapper_C") != nullptr)
    {
        *name = "P50";
        //*color = redcolor;
        return true;
    }
        if (strstr(gname, "_Rifle_M762_Wrapper_C") != nullptr)
    {
        *name = "M762";
        //*color = redcolor;
        return true;
    }
        if (strstr(gname, "_Rifle_AKM_Wrapper_C") != nullptr)
    {
        *name = "AKM";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "_Rifle_AUG_Wrapper_C") != nullptr)
    {
        *name = "AUG";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "_Rifle_MG3_Wrapper_C") != nullptr)
    {
        *name = "MG3";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "_Rifle_HoneyBadger_Wrapper_C") != nullptr)
    {
        *name = "蜜罐";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "_Ammo_556mm_Pickup_C") != nullptr)
    {
        *name = "5.56子弹";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "_Ammo_762mm_Pickup_C") != nullptr)
    {
        *name = "7.62子弹";
        //*color = redcolor;
        return true;
    }
    return false;
}


bool baozinbd(char *gname, char **name) {
     //ALOGD("判断类名%s",ClassName);        
    if (strstr(gname, "ink_Pickup_C") != nullptr)
    {
        *name = "饮料";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "rstaid_Pickup_C") != nullptr)
    {
        *name = "医药包";
        //*color = redcolor;
        return true;
    }
        if (strstr(gname, "lls_Pickup_C") != nullptr)
    {
        *name = "止痛药";
        //*color = redcolor;
        return true;
    }
        if (strstr(gname, "rstAidbox_Pickup_C") != nullptr)
    {
        *name = "医药箱";
        //*color = redcolor;
        return true;
    }
        if (strstr(gname, "_revivalAED_Pickup_C") != nullptr)
    {
        *name = "自救器";
        //*color = redcolor;
        return true;
    }
        if (strstr(gname, "jection_Pickup_C") != nullptr)
    {
        *name = "肾上腺素";
        //*color = redcolor;
        return true;
    }
    return false;
}


bool baozinbc(char *gname, char **name) {
     //ALOGD("判断类名%s",ClassName);        
    if (strstr(gname, "_MZJ_HD_Pickup_C") != nullptr)
    {
        *name = "红点";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "_MZJ_3X_Pickup_C") != nullptr)
    {
        *name = "3倍镜";
        //*color = redcolor;
        return true;
    }
        if (strstr(gname, "_MZJ_6X_Pickup_C") != nullptr)
    {
        *name = "6倍镜";
        //*color = redcolor;
        return true;
    }
        if (strstr(gname, "_DJ_Large_EQ_Pickup_C") != nullptr)
    {
        *name = "快速扩容";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "ckUp_BP_Bag_Lv3_C") != nullptr)
    {
        *name = "三级包";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "ckUp_BP_Armor_Lv3_C") != nullptr)
    {
        *name = "三级甲";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "ckUp_BP_Helmet_Lv3_C") != nullptr)
    {
        *name = "三级头";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "ckUp_BP_Bag_Lv2_C") != nullptr)
    {
        *name = "二级包";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "ckUp_BP_Armor_Lv2_C") != nullptr)
    {
        *name = "二级甲";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "ckUp_BP_Helmet_Lv2_C") != nullptr)
    {
        *name = "二级头";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "_Grenade_Shoulei_Weapon_Wrapper_C") != nullptr)
    {
        *name = "手雷";
        //*color = redcolor;
        return true;
    }
    return false;
}



bool GetGrenadeInfo(char *gname, char **name) {
     //ALOGD("判断类名%s",ClassName);        
    if (strstr(gname, "BP_Grenade_Shoulei_C") != nullptr)//手雷
    {
        *name = "小心有手雷";
        //*color = redcolor;
        return true;
    }
    if (strstr(gname, "BP_Grenade_Burn_C") != nullptr) {
        *name = "小心燃烧瓶";
        //*color = redcolor;
        return true;
    }
    return false;
}

bool GetVehicleInfo(char *gname, char **name) {
     //ALOGD("判断类名%s",ClassName);            
    if (strstr(gname, "_Mountainbike_Training_C") != nullptr) {
        *name = "自行车";
        
        return true;
    }
    if (strstr(gname, "_CoupeRB_1_C") != nullptr) {
        *name = "双人跑车";
        
        return true;
    }
    if (strstr(gname, "_Scooter_C") != nullptr) {
        *name = "小绵羊";
        
        return true;
    }
    if (strstr(gname, "_BRDM_C") != nullptr) {

        *name = "装甲车";
        
        return true;
    }
    if (strstr(gname, "_Motorcycle_1_C")!= nullptr) {

        *name = "摩托车";
        
        return true;
    }
    if (strstr(gname, "_Motorcycle_C") != nullptr) {

        *name = "摩托车";
        
        return true;
    }    
    if (strstr(gname, "_MotorcycleCart_1_C")!= nullptr) {

        *name = "三轮摩托";
        
        return true;
    }

    if (strstr(gname, "_MotorcycleCart_C")!= nullptr) {

        *name = "三轮摩托";
        
        return true;
    }
    if (strstr(gname, "_Snowmobile_C") != nullptr) {

        *name = "雪地摩托";
        
        return true;
    }
    if (strstr(gname, "_Snowbike_02_C") != nullptr) {

        *name = "雪地摩托";
        
        return true;
    }
    if (strstr(gname, "_StationWagon_C") != nullptr) {

        *name = "旅行车";
        
        return true;
    }    
    if (strstr(gname, "_VH_Buggy_C") != nullptr) {

        *name = "蹦蹦车";
        
        return true;
    }
    if (strstr(gname, "_Dacia_New_C") != nullptr) {

        *name = "轿车";
        
        return true;
    }
    if (strstr(gname, "_Dacia_2_New_C") != nullptr) {

        *name = "轿车";
        
        return true;
    }
    if (strstr(gname, "_Dacia_3_New_C") != nullptr) {

        *name = "轿车";
        
        return true;
    }
    if (strstr(gname, "_UAZ01_New_C") != nullptr) {

        *name = "吉普车";
        
        return true;
    }
    if (strstr(gname, "Rony") != nullptr) {

        *name = "皮卡车";
        
        return true;
    }
    if (strstr(gname, "rado_close_1_C") != nullptr) {

        *name = "四人跑车";
        
        return true;
    }
    if (strstr(gname, "rado_open_1_C") != nullptr) {

        *name = "敞篷跑车";
        
        return true;
    }
    if (strstr(gname, "_MiniBus_01_C") != nullptr) {

        *name = "迷你巴士";
        
        return true;
    }
    if (strstr(gname, "_PG117_C") != nullptr) {
        *name = "快艇";
        
        return true;
    }
    if (strstr(gname, "uaRail_1_C") != nullptr) {
        *name = "摩托艇";
        
        return true;
    }
    if (strstr(gname, "_Motorglider_C") != nullptr) {
        *name = "滑翔机";
        
        return true;
    }
    if (strstr(gname, "licedTrain_C") != nullptr) {
        *name = "磁吸小火车";
        
        return true;
    }
    return false;
}
