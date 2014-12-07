package com.cloudmetrics.domain;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;
import javax.persistence.UniqueConstraint;
import javax.xml.bind.annotation.XmlRootElement;
import java.sql.Date;

@Entity
@Table(name="Company", uniqueConstraints=@UniqueConstraint(columnNames={"companyId"}))
@XmlRootElement(name = "Company")
public class Company {
	@Id
	@GeneratedValue(strategy=GenerationType.AUTO)
	@Column
	private Integer companyId;
	
	@Column
	private String companyName;
	
	@Column
	private String password;

	@Column
	private String email;

	public Integer getCompanyId() {
		return companyId;
	}

	public void setCompanyId(Integer companyId) {
		this.companyId = companyId;
	}

	public String getCompanyName() {
		return companyName;
	}

	public void setCompanyName(String companyName) {
		this.companyName = companyName;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public String getEmail() {
		return email;
	}

	public void setEmail(String email) {
		this.email = email;
	}
	
}
