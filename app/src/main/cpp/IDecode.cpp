//
// Created by pwjworks on 2022/7/2.
//

#include "IDecode.h"
#include "XLog.h"

void IDecode::Main() {
    while (!isExit) {
        packsMutex.lock();
        if (packs.empty()) {
            packsMutex.unlock();
            XSleep(1);
            continue;
        }
        // 取出packet，消费者
        XData pack = packs.front();
        packs.pop_front();
        // 发送数据到解码线程，一个数据包，可能解码多个结果
        if (this->SendPacket(pack)) {
            while (!isExit) {
                // 获取解码数据
                XData frame = RecvFrame();
                if (!frame.data) break;
                // 发送数据给观察者
                this->Notify(frame);
            }
        }
        pack.Drop();
        packsMutex.unlock();
    }
}

void IDecode::Update(XData pkt) {
    if (pkt.isAudio != isAudio) {
        return;
    }
    while (!isExit) {
        {
            packsMutex.lock();
            // 大于最大队列值，阻塞
            if (packs.size() < maxList) {
                // 生产者
                packs.emplace_back(pkt);
                packsMutex.unlock();
                break;
            }
        }
        packsMutex.unlock();
        XSleep(1);
    }
}
