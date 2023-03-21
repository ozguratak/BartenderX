//
//  LoginPage.swift
//  Bartender X
//
//  Created by ozgur.atak on 21.03.2023.
//

import UIKit

class LoginPage: UIViewController {

    @IBAction func eyeButtonAction(_ sender: UIButton) {
        if passwordTextField.isSecureTextEntry == true {
            passwordTextField.isSecureTextEntry = false
            eyeButtonOutlet.setImage(UIImage.init(systemName: "eye.slash.fill"), for: .normal)
        } else {
            passwordTextField.isSecureTextEntry = true
            eyeButtonOutlet.setImage(UIImage.init(systemName: "eye.fill"), for: .normal)
        }
    }
    
    @IBAction func forgetPassPressed(_ sender: UIButton) {
        performSegue(withIdentifier: passReset, sender: nil)
    }
    
    @IBAction func registerButtonPressed(_ sender: UIButton) {
        performSegue(withIdentifier: registerSegue, sender: nil)
    }
    
    @IBAction func loginButtonPressed(_ sender: UIButton) {
        let storyboard = UIStoryboard(name: "Main", bundle: Bundle.main)
        let VC = storyboard.instantiateViewController(withIdentifier: tabBarKey)
        navigationController?.setNavigationBarHidden(true, animated: true)
        show(VC, sender: nil)
        
    }
    
    @IBOutlet weak var eyeButtonOutlet: UIButton!
    @IBOutlet weak var passwordTextField: UITextField!
    @IBOutlet weak var emailTextField: UITextField!
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        hideKeyboardWhenTappedAround()
        // Do any additional setup after loading the view.
    }
  
}
