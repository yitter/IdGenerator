module main
import time
import contract
import gen

fn main(){
    // 方法一：直接采用默认方法生成一个Id
	yid := gen.YitIdHelper{
		id_gen: gen.make_generator(&contract.IdGeneratorOptions{
			method: 1
			base_time: 1582136402000
			workerid_bitlength:6
			seq_bitlength:6
		})
	}
	println(yid.next_id())

	// 方法二：自定义参数

	times := 50000

	for {
		begin := time.now().unix_time_milli()
		for i := 0; i < times; i++ {
			yid.next_id()
		}
		end := time.now().unix_time_milli()
		println("漂移，总共："+times.str()+"，"+(end-begin).str()+" ms")
		time.sleep(1 * time.second)
	}
}