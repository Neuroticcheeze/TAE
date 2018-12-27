__kernel void SetBuff( __global float * a_Buff )
{
	const int i = get_global_id( 0 );
	a_Buff[ i ] = pow(1.1, 2.0) - 3 + (i > 0 ? (a_Buff[i-1]):0);
}