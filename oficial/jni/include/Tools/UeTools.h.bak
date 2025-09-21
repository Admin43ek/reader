#pragma once
#ifndef USTOOL_H
#define USTOOL_H

#define PI 3.141592653589793238

float matrix[16] = {0};
double Widtih, Higtih;

typedef char UTF8;
typedef unsigned short UTF16;

struct Actors {
    uint64_t Enc_1, Enc_2;
    uint64_t Enc_3, Enc_4;
};

struct Chunk {
    uint32_t val_1, val_2, val_3, val_4;
    uint32_t val_5, val_6, val_7, val_8;
};

uint64_t DecryptActorsArray(uint64_t uLevel, int Actors_Offset, int EncryptedActors_Offset) {
    if (uLevel < 0x10000000)
        return 0;
 
    if (driver->read<uint64_t>(uLevel + Actors_Offset) > 0)
		return uLevel + Actors_Offset;
 
    if (driver->read<uint64_t>(uLevel + EncryptedActors_Offset) > 0)
		return uLevel + EncryptedActors_Offset;
 
    auto AActors = driver->read<Actors>(uLevel + EncryptedActors_Offset + 0x10);
 
    if (AActors.Enc_1 > 0) {
        auto Enc = driver->read<Chunk>(AActors.Enc_1 + 0x80);
        return (((driver->read<uint8_t>(AActors.Enc_1 + Enc.val_1)
            | (driver->read<uint8_t>(AActors.Enc_1 + Enc.val_2) << 8))
            | (driver->read<uint8_t>(AActors.Enc_1 + Enc.val_3) << 0x10)) & 0xFFFFFF
            | ((uint64_t)driver->read<uint8_t>(AActors.Enc_1 + Enc.val_4) << 0x18)
            | ((uint64_t)driver->read<uint8_t>(AActors.Enc_1 + Enc.val_5) << 0x20)) & 0xFFFF00FFFFFFFFFF
            | ((uint64_t)driver->read<uint8_t>(AActors.Enc_1 + Enc.val_6) << 0x28)
            | ((uint64_t)driver->read<uint8_t>(AActors.Enc_1 + Enc.val_7) << 0x30)
            | ((uint64_t)driver->read<uint8_t>(AActors.Enc_1 + Enc.val_8) << 0x38);
    }
    else if (AActors.Enc_2 > 0) {
        auto Lost_Actors = driver->read<uint64_t>(AActors.Enc_2);
        if (Lost_Actors > 0) {
            return (uint16_t)(Lost_Actors - 0x400) & 0xFF00
                | (uint8_t)(Lost_Actors - 0x04)
                | (Lost_Actors + 0xFC0000) & 0xFF0000
                | (Lost_Actors - 0x4000000) & 0xFF000000
                | (Lost_Actors + 0xFC00000000) & 0xFF00000000
                | (Lost_Actors + 0xFC0000000000) & 0xFF0000000000
                | (Lost_Actors + 0xFC000000000000) & 0xFF000000000000
                | (Lost_Actors - 0x400000000000000) & 0xFF00000000000000;
        }
    }
    else if (AActors.Enc_3 > 0) {
        auto Lost_Actors = driver->read<uint64_t>(AActors.Enc_3);
        if (Lost_Actors > 0) {
            return (Lost_Actors >> 0x38) | (Lost_Actors << (64 - 0x38));
		}
    }
    else if (AActors.Enc_4 > 0) {
        auto Lost_Actors = driver->read<uint64_t>(AActors.Enc_4);
        if (Lost_Actors > 0) {
            return Lost_Actors ^ 0xCDCD00;
		}
    }
    return 0;
}
void GetDistance(Vec3 Object, Vec3 Self, float *Distance)
{
	float DistanceX = pow(Object.x - Self.x, 2);
    float DistanceY = pow(Object.y - Self.y, 2);
    float DistanceZ = pow(Object.z - Self.z, 2);
    *Distance = sqrt(DistanceX + DistanceY + DistanceZ) * 0.01f;
}

void WorldToScreen(float *bscreen, Vec3 *obj)
{
    float camear = matrix[3] * obj->x + matrix[7] * obj->y + matrix[11] * obj->z + matrix[15];
    *bscreen = Higtih - (matrix[1] * obj->x + matrix[5] * obj->y + matrix[9] * obj->z + matrix[13]) / camear * Higtih;
}

void WorldToScreen(Vec2 *bscreen, Vec3 *obj)
{
    float camear = matrix[3] * obj->x + matrix[7] * obj->y + matrix[11] * obj->z + matrix[15];
    bscreen->x = Widtih + (matrix[0] * obj->x + matrix[4] * obj->y + matrix[8] * obj->z + matrix[12]) / camear * Widtih;
    bscreen->y = Higtih - (matrix[1] * obj->x + matrix[5] * obj->y + matrix[9] * obj->z + matrix[13]) / camear * Higtih;
}

void WorldToScreen(Vec2 *bscreen, Vec3 obj)
{
    float camear = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    bscreen->x = Widtih + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear * Widtih;
    bscreen->y = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / camear * Higtih;
}

Vec2 WorldToScreen(Vec3 obj, float a[16], float b)	
{
	Vec2 bscreen;
    float camear = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    bscreen.x = Widtih + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear * Widtih;
    bscreen.y = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / camear * Higtih;
	return bscreen;
}

void WorldToScreen(Vec2 *bscreen, float *camea, Vec3 obj)
{
    float camear = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    *camea = camear;
    bscreen->x = Widtih + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear * Widtih;
    bscreen->y = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / camear * Higtih;
}

void WorldToScreen(Vec2 *bscreen, float *camea, float *w, Vec3 obj)
{
    float camear = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    *camea = camear;
    bscreen->x = Widtih + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear * Widtih;
    bscreen->y = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / camear * Higtih;
	float bscreenZ = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * (obj.z + 165) + matrix[13]) / camear * Higtih;
    float bscreenz = bscreen->y - bscreenZ;
    *w = (bscreen->y - bscreenZ) / 2;
}

void WorldToScreen(Vec4 *bscreen, float *camea, Vec3 obj)
{
    float camear = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    *camea = camear;
    bscreen->x = Widtih + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear * Widtih;
    bscreen->y = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / camear * Higtih;
    float bscreenZ = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * (obj.z + 165) + matrix[13]) / camear * Higtih;
    bscreen->z = bscreen->y - bscreenZ;
    bscreen->w = (bscreen->y - bscreenZ) / 2;
}

Vec2 rotateCoord(float angle, float objRadar_x, float objRadar_y)
{
    Vec2 radarCoordinate;
    float s = sin(angle * PI / 180);
    float c = cos(angle * PI / 180);
    radarCoordinate.x = objRadar_x * c + objRadar_y * s;
    radarCoordinate.y = -objRadar_x * s + objRadar_y * c;
    return radarCoordinate;
}

Vec3 MarixToVector(FMatrix matrix)
{
    return Vec3(matrix.M[3][0], matrix.M[3][1], matrix.M[3][2]);
}

FMatrix MatrixMulti(FMatrix m1, FMatrix m2)
{
    FMatrix matrix = FMatrix();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                matrix.M[i][j] += m1.M[i][k] * m2.M[k][j];
            }
        }
    }
    return matrix;
}

FMatrix TransformToMatrix(FTransform transform)
{
    FMatrix matrix;
    matrix.M[3][0] = transform.Translation.x;
    matrix.M[3][1] = transform.Translation.y;
    matrix.M[3][2] = transform.Translation.z;
    float x2 = transform.Rotation.x + transform.Rotation.x;
    float y2 = transform.Rotation.y + transform.Rotation.y;
    float z2 = transform.Rotation.z + transform.Rotation.z;
    float xx2 = transform.Rotation.x * x2;
    float yy2 = transform.Rotation.y * y2;
    float zz2 = transform.Rotation.z * z2;
    matrix.M[0][0] = (1 - (yy2 + zz2)) * transform.Scale3D.x;
    matrix.M[1][1] = (1 - (xx2 + zz2)) * transform.Scale3D.y;
    matrix.M[2][2] = (1 - (xx2 + yy2)) * transform.Scale3D.z;
    float yz2 = transform.Rotation.y * z2;
    float wx2 = transform.Rotation.w * x2;
    matrix.M[2][1] = (yz2 - wx2) * transform.Scale3D.z;
    matrix.M[1][2] = (yz2 + wx2) * transform.Scale3D.y;
    float xy2 = transform.Rotation.x * y2;
    float wz2 = transform.Rotation.w * z2;
    matrix.M[1][0] = (xy2 - wz2) * transform.Scale3D.y;
    matrix.M[0][1] = (xy2 + wz2) * transform.Scale3D.x;
    float xz2 = transform.Rotation.x * z2;
    float wy2 = transform.Rotation.w * y2;
    matrix.M[2][0] = (xz2 + wy2) * transform.Scale3D.z;
    matrix.M[0][2] = (xz2 - wy2) * transform.Scale3D.x;
    matrix.M[0][3] = 0;
    matrix.M[1][3] = 0;
    matrix.M[2][3] = 0;
    matrix.M[3][3] = 1;
    return matrix;
}

bool GetWether(char *ClassName) {
	if (strstr(ClassName, "DragonBoat") != nullptr)
		return true;
	if (strstr(ClassName, "CoupeRB") != nullptr)
		return true;
	if (strstr(ClassName, "AquaRail") != nullptr)
		return true;
	if (strstr(ClassName, "PG117") != nullptr)
		return true;	
	if (strstr(ClassName, "MiniBus") != nullptr)
		return true;
	if (strstr(ClassName, "Mirado") != nullptr) 
		return true;
	if (strstr(ClassName, "Rony") != nullptr)
		return true;
	if (strstr(ClassName, "_PickUp") != nullptr) 
		return true;
	if (strstr(ClassName, "UAZ") != nullptr)
		return true;
    if (strstr(ClassName, "Dacia") != nullptr)
		return true;
	if (strstr(ClassName, "Buggy") != nullptr)
		return true;
    if (strcmp(ClassName, "BP_VH_Tuk_C") == 0)
		return true;
	if (strcmp(ClassName, "BP_VH_Tuk_1_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_Snowmobile_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_MotorcycleCart_1_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_MotorcycleCart_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_Motorcycle_C") == 0)
		return true;
    if (strcmp(ClassName, "VH_Motorcycle_1_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_Scooter_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_BRDM_C") == 0)
		return true;
	return false;
}

char *dzdz(int dzid)
{
switch (dzid)
    {
        
        case 8203:
        return "边跑边挥拳";
        break;
        
        case 1032:
        case 1033:
        return "探头";
        break;
        
        case 1040:
        case 1041:
        return "蹲着探头";
        break;
        
        case 1544:
        return "[站立]探头开镜";
        break;
        
        case 544:
        return "[趴下]开镜";
        break;
        
        case 1545:
        return "[行走]探头开镜";
        break;
        
        case 1672:
        return "[站立]探头开镜开火";
        break;
        
        case 1673:
        return "[行走]探头开镜开火";
        break;
        
        case 1160:
        return "[站立]探头开火";
        break;
        
        case 1161:
        return "[行走]探头开火";
        break;
        
        case 1168:
        return "[蹲下]探头开火";
        break;
        
        case 1169:
        return "[蹲行]探头开火";
        break;
        
        case 1552:
        return "[蹲下]探头开镜";
        break;
        
        case 1553:
        return "[蹲行]探头开镜";
        break;
        
        case 1680:
        return "[蹲下]探头开火开镜";
        break;
        
        case 1681:
        return "[蹲行]探头开火开镜";
        break;
        
        
        case 329:
        case 328:
        case 288:
      	case 272:
      	case 273:
      	case 264:
      	case 265:
    	return "换弹";
        break;
	
        case 136:
            return "开火";
        break;
        
		case 9:
            return "行走";
        break;
        
        case 524296:
            return "开车";
        break;
        
		case 11:
            return "奔跑";
        break;
        
        case 16777224:
            return "翻墙";
        break;        
        
        case 8:
            return "站立";
        break;  
        
		case 72:	
		case 73:
		case 75:
            return "跳跃";
        break;
        
        case 16:
            return "蹲着";
        break;
        
        case 32:
            return "趴着";
        break;
        
        case 16392:
        case 17416:
        case 16400:
        case 17424:
        case 16416:
        case 16456:
        case 16393:
        case 16457:
            return "拿着投掷物";
        break;
        
        
        case 520:
            return "开镜";
        break;
        
        case 65545:
        return "边走边打药";
        break;
        
        
        case 137:
        return "跑着开枪";
        break;
        
        case 200:
        case 201:
        return "跳枪";
        break;
        
        case 144:
        case 145:
        return "蹲着开枪";
        break;
        
        case 160:
        return "趴着开火";
        break;
        
        case 521:
        return "边开镜边走";
        break;
        
        case 8267:
        return "跳起来挥拳";
        break;
        
        case 8208:
        return "蹲下挥拳";
        break;
        
        case 8211:
        return "边蹲着跑边挥拳";
        break;
    
        case 35:
            return "爬行";
        break;
        
	    case 17:
        return "蹲行";
        break;
        
        case 19:
        return "蹲跑";
        break;
		
		case 268435464:
		return "跳舞";
		break;
		
		case 131072:
		case 524303:
		case 524304:
		case 524289:
            return "倒地";
        break;
        
		case 8200:
            return "挥拳";
        break;
        
        
		case 2056:
		case 2057:
		case 2059:
		case 2065:
		case 2067:
		case 2081:
		case 2080:
		case 2083:
		case 2121:
		case 2120:
		case 2123:
            return "切枪";
        break;
		
		case 4194304:
            return "游泳[未行走]";
        break;
        
        	case 4194307:
            return "游泳[已行走]";
        break;
		
		case 65544:
		case 65552:
		
		case 65568:
		return "打药";  
		break;
		
				
        default:		
            return "未知";
        break;
    }
    return nullptr;
} 

char *GetHol(int Wuqi) {
  switch (Wuqi) {
//突击步枪
    case 101008:
      return "M762";
      break;
    case 1010081:
      return "M762破损";
      break;
    case 1010082:
      return "M762修复";
      break;
    case 1010083:
      return "M762完好";
      break;
    case 1010084:
      return "M762改进";
      break;
    case 1010085:
      return "M762精致";
      break;
    case 1010086:
      return "M762独眼";
      break;
    case 1010087:
      return "M762钢铁";
      break;

    case 101001:
      return "AKM";
      break;
    case 1010011:
      return "AKM破损";
      break;
    case 1010012:
      return "AKM修复";
      break;
    case 1010013:
      return "AKM完好";
      break;
    case 1010014:
      return "AKM改进";
      break;
    case 1010015:
      return "AKM精致";
      break;
    case 1010016:
      return "AKM独眼";
      break;
    case 1010017:
      return "AKM钢铁";
      break;

    case 101004:
      return "M416";
      break;
    case 1010041:
      return "M416破损";
      break;
    case 1010042:
      return "M416修复";
      break;
    case 1010043:
      return "M416完好";
      break;
    case 1010044:
      return "M416改进";
      break;
    case 1010045:
      return "M416精致";
      break;
    case 1010046:
      return "M416独眼";
      break;
    case 1010047:
      return "M416钢铁";
      break;

    case 101003:
      return "SCAR-L";
      break;
    case 1010031:
      return "SCAR-L破损";
      break;
    case 1010032:
      return "SCAR-L修复";
      break;
    case 1010033:
      return "SCAR-L完好";
      break;
    case 1010034:
      return "SCAR-L改进";
      break;
    case 1010035:
      return "SCAR-L精致";
      break;
    case 1010036:
      return "SCAR-L独眼";
      break;
    case 1010037:
      return "SCAR-L钢铁";
      break;

    case 101002:
      return "M16A4";
      break;
    case 1010021:
      return "M16A4破损";
      break;
    case 1010022:
      return "M16A4修复";
      break;
    case 1010023:
      return "M16A4完好";
      break;
    case 1010024:
      return "M16A4改进";
      break;
    case 1010025:
      return "M16A4精致";
      break;
    case 1010026:
      return "M16A4独眼";
      break;
    case 1010027:
      return "M16A4钢铁";
      break;

    case 101009:
      return "Mk47";
      break;
    case 1010091:
      return "Mk47破损";
      break;
    case 1010092:
      return "Mk47修复";
      break;
    case 1010093:
      return "Mk47完好";
      break;
    case 1010094:
      return "Mk47改进";
      break;
    case 1010095:
      return "Mk47精致";
      break;
    case 1010096:
      return "Mk47独眼";
      break;
    case 1010097:
      return "Mk47钢铁";
      break;

    case 101006:
      return "AUG";
      break;
    case 1010061:
      return "AUG破损";
      break;
    case 1010062:
      return "AUG修复";
      break;
    case 1010063:
      return "AUG完好";
      break;
    case 1010064:
      return "AUG改进";
      break;
    case 1010065:
      return "AUG精致";
      break;
    case 1010066:
      return "AUG独眼";
      break;
    case 1010067:
      return "AUG钢铁";
      break;

    case 101005:
      return "Groza";
      break;
    case 1010051:
      return "Groza破损";
      break;
    case 1010052:
      return "Groza修复";
      break;
    case 1010053:
      return "Groza完好";
      break;
    case 1010054:
      return "Groza改进";
      break;
    case 1010055:
      return "Groza精致";
      break;
    case 1010056:
      return "Groza独眼";
      break;
    case 1010057:
      return "Groza钢铁";
      break;

    case 101010:
      return "G36C";
      break;
    case 1010101:
      return "G36C破损";
      break;
    case 1010102:
      return "G36C修复";
      break;
    case 1010103:
      return "G36C完好";
      break;
    case 1010104:
      return "G36C改进";
      break;
    case 1010105:
      return "G36C精致";
      break;
    case 1010106:
      return "G36C独眼";
      break;
    case 1010107:
      return "G36C钢铁";
      break;

    case 101007:
      return "QBZ";
      break;
    case 1010071:
      return "QBZ破损";
      break;
    case 1010072:
      return "QBZ修复";
      break;
    case 1010073:
      return "QBZ完好";
      break;
    case 1010074:
      return "QBZ改进";
      break;
    case 1010075:
      return "QBZ精致";
      break;
    case 1010076:
      return "QBZ独眼";
      break;
    case 1010077:
      return "QBZ钢铁";
      break;

    case 101011:
      return "AC-VAL";
      break;
    case 1010111:
      return "AC-VAL破损";
      break;
    case 1010112:
      return "AC-VAL修复";
      break;
    case 1010113:
      return "AC-VAL完好";
      break;
    case 1010114:
      return "AC-VAL改进";
      break;
    case 1010115:
      return "AC-VAL精致";
      break;
    case 1010116:
      return "AC-VAL独眼";
      break;
    case 1010117:
      return "AC-VAL钢铁";
      break;

    case 101012:
      return "蜜獾突击步枪";
      break;
    case 1010121:
      return "蜜獾突击步枪破损";
      break;
    case 1010122:
      return "蜜獾突击步枪修复";
      break;
    case 1010123:
      return "蜜獾突击步枪完好";
      break;
    case 1010124:
      return "蜜獾突击步枪改进";
      break;
    case 1010125:
      return "蜜獾突击步枪精致";
      break;
    case 1010126:
      return "蜜獾突击步枪独眼";
      break;
    case 1010127:
      return "蜜獾突击步枪钢铁";
      break;
//连狙
    case 103009:
      return "SLR";
      break;
    case 1030091:
      return "SLR破损";
      break;
    case 1030092:
      return "SLR修复";
      break;
    case 1030093:
      return "SLR完好";
      break;
    case 1030094:
      return "SLR改进";
      break;
    case 1030095:
      return "SLR精致";
      break;
    case 1030096:
      return "SLR独眼";
      break;
    case 1030097:
      return "SLR钢铁";
      break;

    case 103005:
      return "VSS";
      break;
    case 1030051:
      return "VSS破损";
      break;
    case 1030052:
      return "VSS修复";
      break;
    case 1030053:
      return "VSS完好";
      break;
    case 1030054:
      return "VSS改进";
      break;
    case 1030055:
      return "VSS精致";
      break;
    case 1030056:
      return "VSS独眼";
      break;
    case 1030057:
      return "VSS钢铁";
      break;

    case 103006:
      return "Mini14";
      break;
    case 1030061:
      return "Mini14破损";
      break;
    case 1030062:
      return "Mini14修复";
      break;
    case 1030063:
      return "Mini14完好";
      break;
    case 1030064:
      return "Mini14改进";
      break;
    case 1030065:
      return "Mini14精致";
      break;
    case 1030066:
      return "Mini14独眼";
      break;
    case 1030067:
      return "Mini14钢铁";
      break;

    case 103010:
      return "QBU";
      break;
    case 1030101:
      return "QBU破损";
      break;
    case 1030102:
      return "QBU修复";
      break;
    case 1030103:
      return "QBU完好";
      break;
    case 1030104:
      return "QBU改进";
      break;
    case 1030105:
      return "QBU精致";
      break;
    case 1030106:
      return "QBU独眼";
      break;
    case 1030107:
      return "QBU钢铁";
      break;

    case 103004:
      return "SKS";
      break;
    case 1030041:
      return "SKS破损";
      break;
    case 1030042:
      return "SKS修复";
      break;
    case 1030043:
      return "SKS完好";
      break;
    case 1030044:
      return "SKS改进";
      break;
    case 1030045:
      return "SKS精致";
      break;
    case 1030046:
      return "SKS独眼";
      break;
    case 1030047:
      return "SKS钢铁";
      break;

    case 103007:
      return "MK14";
      break;
    case 1030071:
      return "MK14破损";
      break;
    case 1030072:
      return "MK14修复";
      break;
    case 1030073:
      return "MK14完好";
      break;
    case 1030074:
      return "MK14改进";
      break;
    case 1030075:
      return "MK14精致";
      break;
    case 1030076:
      return "MK14独眼";
      break;
    case 1030077:
      return "MK14钢铁";
      break;

    case 103014:
      return "MK20-H";
      break;
    case 1030141:
      return "MK20-H破损";
      break;
    case 1030142:
      return "MK20-H修复";
      break;
    case 1030143:
      return "MK20-H完好";
      break;
    case 1030144:
      return "MK20-H改进";
      break;
    case 1030145:
      return "MK20-H精致";
      break;
    case 1030146:
      return "MK20-H独眼";
      break;
    case 1030147:
      return "MK20-H钢铁";
      break;

    case 103013:
      return "M4117";
      break;
    case 1030131:
      return "M4117破损";
      break;
    case 1030132:
      return "M4117修复";
      break;
    case 1030133:
      return "M4117完好";
      break;
    case 1030134:
      return "M4117改进";
      break;
    case 1030135:
      return "M4117精致";
      break;
    case 1030136:
      return "M4117独眼";
      break;
    case 1030137:
      return "M4117钢铁";
      break;
//连狙
    case 103012:
      return "ARM";
      break;
    case 1030121:
      return "ARM破损";
      break;
    case 1030122:
      return "ARM修复";
      break;
    case 1030123:
      return "ARM完好";
      break;
    case 1030124:
      return "ARM改进";
      break;
    case 1030125:
      return "ARM精致";
      break;
    case 1030126:
      return "ARM独眼";
      break;
    case 1030127:
      return "ARM钢铁";
      break;

    case 103003:
      return "AWM";
      break;
    case 1030031:
      return "AWM破损";
      break;
    case 1030032:
      return "AWM修复";
      break;
    case 1030033:
      return "AWM完好";
      break;
    case 1030034:
      return "AWM改进";
      break;
    case 1030035:
      return "AWM精致";
      break;
    case 1030036:
      return "AWM独眼";
      break;
    case 1030037:
      return "AWM钢铁";
      break;

    case 103002:
      return "M24";
      break;
    case 1030021:
      return "M24破损";
      break;
    case 1030022:
      return "M24修复";
      break;
    case 1030023:
      return "M24完好";
      break;
    case 1030024:
      return "M24改进";
      break;
    case 1030025:
      return "M24精致";
      break;
    case 1030026:
      return "M24独眼";
      break;
    case 1030027:
      return "M24钢铁";
      break;

    case 103011:
      return "莫幸纳甘";
      break;
    case 1030111:
      return "莫幸纳甘破损";
      break;
    case 1030112:
      return "莫幸纳甘修复";
      break;
    case 1030113:
      return "莫幸纳甘完好";
      break;
    case 1030114:
      return "莫幸纳甘改进";
      break;
    case 1030115:
      return "莫幸纳甘精致";
      break;
    case 1030116:
      return "莫幸纳甘独眼";
      break;
    case 1030117:
      return "莫幸纳甘钢铁";
      break;

    case 103001:
      return "Kar98K";
      break;
    case 1030011:
      return "Kar98K破损";
      break;
    case 1030012:
      return "Kar98K修复";
      break;
    case 1030013:
      return "Kar98K完好";
      break;
    case 1030014:
      return "Kar98K改进";
      break;
    case 1030015:
      return "Kar98K精致";
      break;
    case 1030016:
      return "Kar98K独眼";
      break;
    case 1030017:
      return "Kar98K钢铁";
      break;

    case 103008:
      return "Win94";
      break;
    case 1030081:
      return "Win94破损";
      break;
    case 1030082:
      return "Win94修复";
      break;
    case 1030083:
      return "Win94完好";
      break;
    case 1030084:
      return "Win94改进";
      break;
    case 1030085:
      return "Win94精致";
      break;
    case 1030086:
      return "Win94独眼";
      break;
    case 1030087:
      return "Win94钢铁";
      break;
//机关枪
    case 105001:
      return "M249";
      break;
    case 1050011:
      return "M249破损";
      break;
    case 1050012:
      return "M249修复";
      break;
    case 1050013:
      return "M249完好";
      break;
    case 1050014:
      return "M249改进";
      break;
    case 1050015:
      return "M249精致";
      break;
    case 1050016:
      return "M249独眼";
      break;
    case 1050017:
      return "M249钢铁";
      break;

    case 105002:
      return "DP-28";
      break;
    case 1050021:
      return "DP-28破损";
      break;
    case 1050022:
      return "DP-28修复";
      break;
    case 1050023:
      return "DP-28完好";
      break;
    case 1050024:
      return "DP-28改进";
      break;
    case 1050025:
      return "DP-28精致";
      break;
    case 1050026:
      return "DP-28独眼";
      break;
    case 1050027:
      return "DP-28钢铁";
      break;

    case 105010:
      return "MG3";
      break;
    case 1050101:
      return "MG3破损";
      break;
    case 1050102:
      return "MG3修复";
      break;
    case 1050103:
      return "MG3完好";
      break;
    case 1050104:
      return "MG3改进";
      break;
    case 1050105:
      return "MG3精致";
      break;
    case 1050106:
      return "MG3独眼";
      break;
    case 1050107:
      return "MG3钢铁";
      break;

    case 107001:
      return "十字弩";
      break;
    case 1070011:
      return "十字弩破损";
      break;
    case 1070012:
      return "十字弩修复";
      break;
    case 1070013:
      return "十字弩完好";
      break;
    case 1070014:
      return "十字弩改进";
      break;
    case 1070015:
      return "十字弩精致";
      break;
    case 1070016:
      return "十字弩独眼";
      break;
    case 1070017:
      return "十字弩钢铁";
      break;

    case 107007:
      return "爆炸猎弓";
      break;
    case 1070071:
      return "爆炸猎弓破损";
      break;
    case 1070072:
      return "爆炸猎弓修复";
      break;
    case 1070073:
      return "爆炸猎弓完好";
      break;
    case 1070074:
      return "爆炸猎弓改进";
      break;
    case 1070075:
      return "爆炸猎弓精致";
      break;
    case 1070076:
      return "爆炸猎弓独眼";
      break;
    case 1070077:
      return "爆炸猎弓钢铁";
      break;
//冲锋枪
    case 102001:
      return "UZI";
      break;
    case 1020011:
      return "UZI破损";
      break;
    case 1020012:
      return "UZI修复";
      break;
    case 1020013:
      return "UZI完好";
      break;
    case 1020014:
      return "UZI改进";
      break;
    case 1020015:
      return "UZI精致";
      break;
    case 1020016:
      return "UZI独眼";
      break;
    case 1020017:
      return "UZI钢铁";
      break;

    case 102003:
      return "Vector";
      break;
    case 1020031:
      return "Vector破损";
      break;
    case 1020032:
      return "Vector修复";
      break;
    case 1020033:
      return "Vector完好";
      break;
    case 1020034:
      return "Vector改进";
      break;
    case 1020035:
      return "Vector精致";
      break;
    case 1020036:
      return "Vector独眼";
      break;
    case 1020037:
      return "Vector钢铁";
      break;

    case 100103:
      return "PP-19";
      break;
    case 1001031:
      return "PP-19破损";
      break;
    case 1001032:
      return "PP-19修复";
      break;
    case 1001033:
      return "PP-19完好";
      break;
    case 1001034:
      return "PP-19改进";
      break;
    case 1001035:
      return "PP-19精致";
      break;
    case 1001036:
      return "PP-19独眼";
      break;
    case 1001037:
      return "PP-19钢铁";
      break;

    case 102007:
      return "MP5K";
      break;
    case 1020071:
      return "MP5K破损";
      break;
    case 1020072:
      return "MP5K修复";
      break;
    case 1020073:
      return "MP5K完好";
      break;
    case 1020074:
      return "MP5K改进";
      break;
    case 1020075:
      return "MP5K精致";
      break;
    case 1020076:
      return "MP5K独眼";
      break;
    case 1020077:
      return "MP5K钢铁";
      break;

    case 102002:
      return "UMP-45";
      break;
    case 1020021:
      return "UMP-45破损";
      break;
    case 1020022:
      return "UMP-45修复";
      break;
    case 1020023:
      return "UMP-45完好";
      break;
    case 1020024:
      return "UMP-45改进";
      break;
    case 1020025:
      return "UMP-45精致";
      break;
    case 1020026:
      return "UMP-45独眼";
      break;
    case 1020027:
      return "UMP-45钢铁";
      break;

    case 102004:
      return "汤姆逊";
      break;
    case 1020041:
      return "汤姆逊破损";
      break;
    case 1020042:
      return "汤姆逊修复";
      break;
    case 1020043:
      return "汤姆逊完好";
      break;
    case 1020044:
      return "汤姆逊改进";
      break;
    case 1020045:
      return "汤姆逊精致";
      break;
    case 1020046:
      return "汤姆逊独眼";
      break;
    case 1020047:
      return "汤姆逊钢铁";
      break;

    case 102105:
      return "P90";
      break;
    case 1021051:
      return "P90破损";
      break;
    case 1021052:
      return "P90修复";
      break;
    case 1021053:
      return "P90完好";
      break;
    case 1021054:
      return "P90改进";
      break;
    case 1021055:
      return "P90精致";
      break;
    case 1021056:
      return "P90独眼";
      break;
    case 1021057:
      return "P90钢铁";
      break;

    case 102005:
      return "野牛";
      break;
    case 1020051:
      return "野牛破损";
      break;
    case 1020052:
      return "野牛修复";
      break;
    case 1020053:
      return "野牛完好";
      break;
    case 1020054:
      return "野牛改进";
      break;
    case 1020055:
      return "野牛精致";
      break;
    case 1020056:
      return "野牛独眼";
      break;
    case 1020057:
      return "野牛钢铁";
      break;
//霰弹枪
    case 104001:
      return "S686";
      break;
    case 1040011:
      return "S686破损";
      break;
    case 1040012:
      return "S686修复";
      break;
    case 1040013:
      return "S686完好";
      break;
    case 1040014:
      return "S686改进";
      break;
    case 1040015:
      return "S686精致";
      break;
    case 1040016:
      return "S686独眼";
      break;
    case 1040017:
      return "S686钢铁";
      break;

    case 104002:
      return "S1897";
      break;
    case 1040021:
      return "S1897破损";
      break;
    case 1040022:
      return "S1897修复";
      break;
    case 1040023:
      return "S1897完好";
      break;
    case 1040024:
      return "S1897改进";
      break;
    case 1040025:
      return "S1897精致";
      break;
    case 1040026:
      return "S1897独眼";
      break;
    case 1040027:
      return "S1897钢铁";
      break;

    case 104003:
      return "S12K";
      break;
    case 1040031:
      return "S12K破损";
      break;
    case 1040032:
      return "S12K修复";
      break;
    case 1040033:
      return "S12K完好";
      break;
    case 1040034:
      return "S12K改进";
      break;
    case 1040035:
      return "S12K精致";
      break;
    case 1040036:
      return "S12K独眼";
      break;
    case 1040037:
      return "S12K钢铁";
      break;

    case 104004:
      return "DBS";
      break;
    case 1040041:
      return "DBS破损";
      break;
    case 1040042:
      return "DBS修复";
      break;
    case 1040043:
      return "DBS完好";
      break;
    case 1040044:
      return "DBS改进";
      break;
    case 1040045:
      return "DBS精致";
      break;
    case 1040046:
      return "DBS独眼";
      break;
    case 1040047:
      return "DBS钢铁";
      break;

    case 104100:
      return "SPAS-12";
      break;
    case 1041001:
      return "SPAS-12破损";
      break;
    case 1041002:
      return "SPAS-12修复";
      break;
    case 1041003:
      return "SPAS-12完好";
      break;
    case 1041004:
      return "SPAS-12改进";
      break;
    case 1041005:
      return "SPAS-12精致";
      break;
    case 1041006:
      return "SPAS-12独眼";
      break;
    case 1041007:
      return "SPAS-12钢铁";
      break;
//投掷爆炸物
    case 602004:
      return "手榴弹";
      break;
    case 602003:
      return "燃烧瓶";
      break;
    case 602002:
      return "烟雾弹";
      break;
    case 602001:
      return "震撼弹";
      break;
//近战武器
    case 108003:
      return "镰刀";
      break;
    case 108002:
      return "撬棍";
      break;
    case 108001:
      return "大砍刀";
      break;
    case 108004:
      return "平底锅";
      break;
    case 0:
      return "拳头";
      break;
    default:
      return "未收录";
      break;
  }
  return nullptr;
} 

#endif
