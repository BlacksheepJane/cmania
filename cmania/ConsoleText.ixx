module;
#pragma warning(disable:5260)
export module ConsoleText;
import <initializer_list>;
import <exception>;
import <list>;

struct CharMeasureData
{
public:
	int Start;
	int End;
	unsigned char Length;
	int InRange(int c)
	{
		if (c < Start)
			return c - Start;
		else if (c >= End)
			return End - c + 1;
		return 0;
	}
};
struct CharMeasureDataCollection
{
	CharMeasureData* data;
	size_t size;
public:
	CharMeasureDataCollection(std::initializer_list<CharMeasureData> list)
	{
		size = list.size();
		data = new CharMeasureData[size];
		std::copy(list.begin(), list.end(), data);
	}
	int Search(int c) const
	{
		int low = 0;
		int high = size;
		int middle = 0;
		auto ptr = data;
		while (low <= high)
		{
			middle = (low + high) / 2;
			auto curSeg = ptr[middle];
			if (c < curSeg.Start)
			{
				high = middle - 1;
			}
			else if (c < curSeg.End)
			{
				return curSeg.Length;
			}
			else
			{
				low = middle + 1;
			}
		}
		return -1;
	}
};
CharMeasureDataCollection col{ {0,7,1},{7,9,0},{9,10,8},{10,11,0},{11,13,1},{13,14,0},{14,162,1},{162,166,2},{166,167,1},{167,169,2},{169,175,1},{175,178,2},{178,180,1},{180,182,2},{182,183,1},{183,184,2},{184,215,1},{215,216,2},{216,247,1},{247,248,2},{248,449,1},{449,450,2},{450,711,1},{711,712,2},{712,713,1},{713,716,2},{716,729,1},{729,730,2},{730,913,1},{913,930,2},{930,931,1},{931,938,2},{938,945,1},{945,962,2},{962,963,1},{963,970,2},{970,1025,1},{1025,1026,2},{1026,1040,1},{1040,1104,2},{1104,1105,1},{1105,1106,2},{1106,8208,1},{8208,8209,2},{8209,8211,1},{8211,8215,2},{8215,8216,1},{8216,8218,2},{8218,8220,1},{8220,8222,2},{8222,8229,1},{8229,8231,2},{8231,8240,1},{8240,8241,2},{8241,8242,1},{8242,8244,2},{8244,8245,1},{8245,8246,2},{8246,8251,1},{8251,8252,2},{8252,8254,1},{8254,8255,2},{8255,8364,1},{8364,8365,2},{8365,8451,1},{8451,8452,2},{8452,8453,1},{8453,8454,2},{8454,8457,1},{8457,8458,2},{8458,8470,1},{8470,8471,2},{8471,8481,1},{8481,8482,2},{8482,8544,1},{8544,8556,2},{8556,8560,1},{8560,8570,2},{8570,8592,1},{8592,8596,2},{8596,8598,1},{8598,8602,2},{8602,8712,1},{8712,8713,2},{8713,8719,1},{8719,8720,2},{8720,8721,1},{8721,8722,2},{8722,8725,1},{8725,8726,2},{8726,8728,1},{8728,8729,2},{8729,8730,1},{8730,8731,2},{8731,8733,1},{8733,8737,2},{8737,8739,1},{8739,8740,2},{8740,8741,1},{8741,8742,2},{8742,8743,1},{8743,8748,2},{8748,8750,1},{8750,8751,2},{8751,8756,1},{8756,8760,2},{8760,8764,1},{8764,8766,2},{8766,8776,1},{8776,8777,2},{8777,8780,1},{8780,8781,2},{8781,8786,1},{8786,8787,2},{8787,8800,1},{8800,8802,2},{8802,8804,1},{8804,8808,2},{8808,8814,1},{8814,8816,2},{8816,8853,1},{8853,8854,2},{8854,8857,1},{8857,8858,2},{8858,8869,1},{8869,8870,2},{8870,8895,1},{8895,8896,2},{8896,8978,1},{8978,8979,2},{8979,9312,1},{9312,9322,2},{9322,9332,1},{9332,9372,2},{9372,9632,1},{9632,9634,2},{9634,9650,1},{9650,9652,2},{9652,9660,1},{9660,9662,2},{9662,9670,1},{9670,9672,2},{9672,9675,1},{9675,9676,2},{9676,9678,1},{9678,9680,2},{9680,9698,1},{9698,9702,2},{9702,9733,1},{9733,9735,2},{9735,9737,1},{9737,9738,2},{9738,9792,1},{9792,9793,2},{9793,9794,1},{9794,9795,2},{9795,12288,1},{12288,12292,2},{12292,12293,1},{12293,12312,2},{12312,12317,1},{12317,12319,2},{12319,12321,1},{12321,12330,2},{12330,12353,1},{12353,12436,2},{12436,12443,1},{12443,12447,2},{12447,12449,1},{12449,12535,2},{12535,12540,1},{12540,12543,2},{12543,12549,1},{12549,12586,2},{12586,12690,1},{12690,12704,2},{12704,12832,1},{12832,12868,2},{12868,12928,1},{12928,12958,2},{12958,12959,1},{12959,12964,2},{12964,12969,1},{12969,12977,2},{12977,13198,1},{13198,13200,2},{13200,13212,1},{13212,13215,2},{13215,13217,1},{13217,13218,2},{13218,13252,1},{13252,13253,2},{13253,13262,1},{13262,13263,2},{13263,13265,1},{13265,13267,2},{13267,13269,1},{13269,13270,2},{13270,19968,1},{19968,40870,2},{40870,55296,1},{55296,55297,0},{55297,56320,1},{56320,56321,2},{56321,57344,1},{57344,59335,2},{59335,59337,1},{59337,59493,2},{59493,63733,1},{63733,63734,2},{63734,63744,1},{63744,64046,2},{64046,65072,1},{65072,65074,2},{65074,65075,1},{65075,65093,2},{65093,65097,1},{65097,65107,2},{65107,65108,1},{65108,65112,2},{65112,65113,1},{65113,65127,2},{65127,65128,1},{65128,65132,2},{65132,65281,1},{65281,65375,2},{65375,65504,1},{65504,65510,2},{65510,65536,1} };
export int Measure(int ucs4)
{
	return col.Search(ucs4);
}