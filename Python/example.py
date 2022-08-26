from source import options, generator, idregister

if __name__ == '__main__':
    try:
        # 连接redis
        register = idregister.Register(host="127.0.0.1", port=6379)

        # 获取worker id
        worker_id = register.get_worker_id()

        # 生成id generator
        options = options.IdGeneratorOptions(worker_id=worker_id, seq_bit_length=10)
        options.base_time = 12311111112
        idgen = generator.DefaultIdGenerator()
        idgen.set_id_generator(options)

        uid = idgen.next_id()

        print(worker_id)
        print(uid)
        print(options.__dict__)

        # 退出注册器线程
        register.stop()

    except ValueError as e:
        print(e)
