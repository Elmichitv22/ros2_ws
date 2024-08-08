#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "turtlesim/srv/set_pen.hpp"
#include "turtlesim/msg/pose.hpp"

class ServicePractice : public rclcpp::Node {
public:
    ServicePractice() : Node("service_practice"), is_red_(false) {
        cli_ = this->create_client<turtlesim::srv::SetPen>("turtle1/set_pen");
        while (!cli_->wait_for_service(std::chrono::seconds(1))) {
            RCLCPP_INFO(this->get_logger(), "Service not available, waiting again...");
        }
        subscription_ = this->create_subscription<turtlesim::msg::Pose>(
            "turtle1/pose",
            10,
            std::bind(&ServicePractice::listener_callback, this, std::placeholders::_1)
        );
    }

private:
    void send_request(uint8_t r, uint8_t g, uint8_t b) {
        auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
        request->r = r;
        request->g = g;
        request->b = b;
        request->width = 3;
        request->off = 0;
        auto result = cli_->async_send_request(request);
    }

    void listener_callback(const turtlesim::msg::Pose::SharedPtr msg) {
        if (msg->x > 5.5) {
            if (!is_red_) {
                RCLCPP_INFO(this->get_logger(), "Set pen color to red");
                send_request(255, 0, 0);
                is_red_ = true;
            }
        } else {
            if (is_red_) {
                RCLCPP_INFO(this->get_logger(), "Set pen color to green");
                send_request(0, 255, 0);
                is_red_ = false;
            }
        }
    }

    rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr cli_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
    bool is_red_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ServicePractice>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
