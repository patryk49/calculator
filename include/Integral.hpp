#pragma once

#include "Utils.hpp"

namespace sp{



constexpr const double QuadratureNodes[] = {
	0.0243502926634244,
	0.0729931217877990,
	0.1214628192961206,
	0.1696444204239928,
	0.2174236437400071,
	0.2646871622087674,
	0.3113228719902110,
	0.3572201583376681,
	0.4022701579639916,
	0.4463660172534641,
	0.4894031457070530,
	0.5312794640198946,
	0.5718956462026340,
	0.6111553551723933,
	0.6489654712546573,
	0.6852363130542333,
	0.7198818501716109,
	0.7528199072605319,
	0.7839723589433414,
	0.8132653151227975,
	0.8406292962525803,
	0.8659993981540928,
	0.8893154459951141,
	0.9105221370785028,
	0.9295691721319396,
	0.9464113748584028,
	0.9610087996520538,
	0.9733268277899110,
	0.9833362538846260,
	0.9910133714767443,
	0.9963401167719553,
	0.9993050417357722
};


constexpr const double QuadratureWeights[] = {
	0.0486909570091397,
	0.0485754674415034,
	0.0483447622348030,
	0.0479993885964583,
	0.0475401657148303,
	0.0469681828162100,
	0.0462847965813144,
	0.0454916279274181,
	0.0445905581637566,
	0.0435837245293235,
	0.0424735151236536,
	0.0412625632426235,
	0.0399537411327203,
	0.0385501531786156,
	0.0370551285402400,
	0.0354722132568824,
	0.0338051618371416,
	0.0320579283548516,
	0.0302346570724025,
	0.0283396726142595,
	0.0263774697150547,
	0.0243527025687109,
	0.0222701738083833,
	0.0201348231535302,
	0.0179517157756973,
	0.0157260304760247,
	0.0134630478967186,
	0.0111681394601311,
	0.0088467598263639,
	0.0065044579689784,
	0.0041470332605625,
	0.0017832807216964
};

template<class Callable>
constexpr double gaussianQuadrature(const double begin, const double end, Callable function) noexcept{
	const double scale = (end - begin) / 2.0;
	const double midpoint = (end + begin) / 2.0;

	//	double sum = 0.0;

	// for (size_t i=0; i!=32; i+=1){
	// 	const double offset0 = QuadratureNodes[i] * scale;
	// 	const double res0 = (function(midpoint-offset0) + function(midpoint+offset0)) * QuadratureWeights[i];
	// 	// const double offset1 = QuadratureNodes[i+1] * scale;
	// 	// const double res1 = (function(midpoint-offset1) + function(midpoint+offset1)) * QuadratureWeights[i+1];
	// 	sum += res0;// + res1;
	// }


	double sum =
	       (function(midpoint-QuadratureNodes[ 0]*scale) + function(midpoint+QuadratureNodes[ 0]*scale)) * QuadratureWeights[ 0];
	sum += (function(midpoint-QuadratureNodes[ 1]*scale) + function(midpoint+QuadratureNodes[ 1]*scale)) * QuadratureWeights[ 1];
	sum += (function(midpoint-QuadratureNodes[ 2]*scale) + function(midpoint+QuadratureNodes[ 2]*scale)) * QuadratureWeights[ 2];
	sum += (function(midpoint-QuadratureNodes[ 3]*scale) + function(midpoint+QuadratureNodes[ 3]*scale)) * QuadratureWeights[ 3];
	sum += (function(midpoint-QuadratureNodes[ 4]*scale) + function(midpoint+QuadratureNodes[ 4]*scale)) * QuadratureWeights[ 4];
	sum += (function(midpoint-QuadratureNodes[ 5]*scale) + function(midpoint+QuadratureNodes[ 5]*scale)) * QuadratureWeights[ 5];
	sum += (function(midpoint-QuadratureNodes[ 6]*scale) + function(midpoint+QuadratureNodes[ 6]*scale)) * QuadratureWeights[ 6];
	sum += (function(midpoint-QuadratureNodes[ 7]*scale) + function(midpoint+QuadratureNodes[ 7]*scale)) * QuadratureWeights[ 7];
	sum += (function(midpoint-QuadratureNodes[ 8]*scale) + function(midpoint+QuadratureNodes[ 8]*scale)) * QuadratureWeights[ 8];
	sum += (function(midpoint-QuadratureNodes[ 9]*scale) + function(midpoint+QuadratureNodes[ 9]*scale)) * QuadratureWeights[ 9];
	sum += (function(midpoint-QuadratureNodes[10]*scale) + function(midpoint+QuadratureNodes[10]*scale)) * QuadratureWeights[10];
	sum += (function(midpoint-QuadratureNodes[11]*scale) + function(midpoint+QuadratureNodes[11]*scale)) * QuadratureWeights[11];
	sum += (function(midpoint-QuadratureNodes[12]*scale) + function(midpoint+QuadratureNodes[12]*scale)) * QuadratureWeights[12];
	sum += (function(midpoint-QuadratureNodes[13]*scale) + function(midpoint+QuadratureNodes[13]*scale)) * QuadratureWeights[13];
	sum += (function(midpoint-QuadratureNodes[14]*scale) + function(midpoint+QuadratureNodes[14]*scale)) * QuadratureWeights[14];
	sum += (function(midpoint-QuadratureNodes[15]*scale) + function(midpoint+QuadratureNodes[15]*scale)) * QuadratureWeights[15];
	sum += (function(midpoint-QuadratureNodes[16]*scale) + function(midpoint+QuadratureNodes[16]*scale)) * QuadratureWeights[16];
	sum += (function(midpoint-QuadratureNodes[17]*scale) + function(midpoint+QuadratureNodes[17]*scale)) * QuadratureWeights[17];
	sum += (function(midpoint-QuadratureNodes[18]*scale) + function(midpoint+QuadratureNodes[18]*scale)) * QuadratureWeights[18];
	sum += (function(midpoint-QuadratureNodes[19]*scale) + function(midpoint+QuadratureNodes[19]*scale)) * QuadratureWeights[19];
	sum += (function(midpoint-QuadratureNodes[20]*scale) + function(midpoint+QuadratureNodes[20]*scale)) * QuadratureWeights[20];
	sum += (function(midpoint-QuadratureNodes[21]*scale) + function(midpoint+QuadratureNodes[21]*scale)) * QuadratureWeights[21];
	sum += (function(midpoint-QuadratureNodes[22]*scale) + function(midpoint+QuadratureNodes[22]*scale)) * QuadratureWeights[22];
	sum += (function(midpoint-QuadratureNodes[23]*scale) + function(midpoint+QuadratureNodes[23]*scale)) * QuadratureWeights[23];
	sum += (function(midpoint-QuadratureNodes[24]*scale) + function(midpoint+QuadratureNodes[24]*scale)) * QuadratureWeights[24];
	sum += (function(midpoint-QuadratureNodes[25]*scale) + function(midpoint+QuadratureNodes[25]*scale)) * QuadratureWeights[25];
	sum += (function(midpoint-QuadratureNodes[26]*scale) + function(midpoint+QuadratureNodes[26]*scale)) * QuadratureWeights[26];
	sum += (function(midpoint-QuadratureNodes[27]*scale) + function(midpoint+QuadratureNodes[27]*scale)) * QuadratureWeights[27];
	sum += (function(midpoint-QuadratureNodes[28]*scale) + function(midpoint+QuadratureNodes[28]*scale)) * QuadratureWeights[28];
	sum += (function(midpoint-QuadratureNodes[29]*scale) + function(midpoint+QuadratureNodes[29]*scale)) * QuadratureWeights[29];
	sum += (function(midpoint-QuadratureNodes[30]*scale) + function(midpoint+QuadratureNodes[30]*scale)) * QuadratureWeights[30];
	sum += (function(midpoint-QuadratureNodes[31]*scale) + function(midpoint+QuadratureNodes[31]*scale)) * QuadratureWeights[31];

	return sum * scale;
}

} // END OF NAMESPACE ///////////////////////////////////////
